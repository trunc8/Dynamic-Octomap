// trunc8 did this

#include "octomap/DoseOcTree.h"

namespace octomap {

	// node implementation  --------------------------------------
  std::ostream& DoseOcTreeNode::writeData(std::ostream &s) const {
    s.write((const char*) &value, sizeof(value)); // occupancy
    s.write((const char*) &dose, sizeof(dose)); // dose

    return s;
  }

  std::istream& DoseOcTreeNode::readData(std::istream &s) {
    s.read((char*) &value, sizeof(value)); // occupancy
    s.read((char*) &dose, sizeof(dose)); // dose

    return s;
  }

  float DoseOcTreeNode::getAverageChildDose() const {
    float mdose = 0;
    int c = 0;

    if (children != NULL){
      for (int i=0; i<8; i++) {
        DoseOcTreeNode* child = static_cast<DoseOcTreeNode*>(children[i]);

        if (child != NULL && child->isDoseSet()) {
          mdose += child->getDose();
          ++c;
        }
      }
    }

    if (c > 0) {
      mdose /= c;
      return mdose;
    }
    else { // no child had the dose set
      return 0;
    }
  }


  void DoseOcTreeNode::updateDoseChildren() {
    dose = getAverageChildDose();
  }

  // tree implementation  --------------------------------------
  DoseOcTree::DoseOcTree(double in_resolution)
   : OccupancyOcTreeBase<DoseOcTreeNode>(in_resolution) {
    doseOcTreeMemberInit.ensureLinking();
  }

  DoseOcTreeNode* DoseOcTree::setNodeDose(const OcTreeKey& key,
                                             float d) {
    DoseOcTreeNode* n = search (key);
    if (n != 0) {
      n->setDose(d);
    }
    return n;
  }

  bool DoseOcTree::pruneNode(DoseOcTreeNode* node) {
    if (!isNodeCollapsible(node))
      return false;

    // set value to children's values (all assumed equal)
    node->copyData(*(getNodeChild(node, 0)));

    if (node->isDoseSet()) // TODO check
      node->setDose(node->getAverageChildDose());

    // delete children
    for (unsigned int i=0;i<8;i++) {
      deleteNodeChild(node, i);
    }
    delete[] node->children;
    node->children = NULL;

    return true;
  }

  bool DoseOcTree::isNodeCollapsible(const DoseOcTreeNode* node) const{
    // all children must exist, must not have children of
    // their own and have the same occupancy probability
    if (!nodeChildExists(node, 0))
      return false;

    const DoseOcTreeNode* firstChild = getNodeChild(node, 0);
    if (nodeHasChildren(firstChild))
      return false;

    for (unsigned int i = 1; i<8; i++) {
      // compare nodes only using their occupancy, ignoring dose for pruning
      if (!nodeChildExists(node, i) || nodeHasChildren(getNodeChild(node, i)) || !(getNodeChild(node, i)->getValue() == firstChild->getValue()))
        return false;
    }

    return true;
  }

  DoseOcTreeNode* DoseOcTree::incrementNodeDose(const OcTreeKey& key,
                                                 float d) {
    DoseOcTreeNode* n = search(key);
    if (n != 0) {
      if (n->isDoseSet()) {
        float prev_dose = n->getDose();
        if (prev_dose < 1000) {
          n->setDose((prev_dose + d));
        }
      }
      else {
        n->setDose(d);
      }
    }
    return n;
  }

  DoseOcTreeNode* DoseOcTree::integrateNodeDose(const OcTreeKey& key,
                                                   float d) {
    DoseOcTreeNode* n = search (key);
    if (n != 0) {
      if (n->isDoseSet()) {
        float prev_dose = n->getDose();
        double node_prob = n->getOccupancy();
        float new_dose = ((double) prev_dose * node_prob
                           +  (double) d * (0.99-node_prob));
        n->setDose(new_dose);
      }
      else {
        n->setDose(d);
      }
    }
    return n;
  }


  void DoseOcTree::updateInnerOccupancy() {
    this->updateInnerOccupancyRecurs(this->root, 0);
  }

  void DoseOcTree::updateInnerOccupancyRecurs(DoseOcTreeNode* node, unsigned int depth) {
    // only recurse and update for inner nodes:
    if (nodeHasChildren(node)){
      // return early for last level:
      if (depth < this->tree_depth){
        for (unsigned int i=0; i<8; i++) {
          if (nodeChildExists(node, i)) {
            updateInnerOccupancyRecurs(getNodeChild(node, i), depth+1);
          }
        }
      }
      node->updateOccupancyChildren();
      node->updateDoseChildren();
    }
  }

  DoseOcTree::StaticMemberInitializer DoseOcTree::doseOcTreeMemberInit;

} // end namespace
