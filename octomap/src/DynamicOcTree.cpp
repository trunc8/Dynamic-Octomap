// trunc8 did this

#include "octomap/DynamicOcTree.h"

namespace octomap {

  // node implementation  --------------------------------------
  std::ostream& DynamicOcTreeNode::writeData(std::ostream &s) const {
    s.write((const char*) &value, sizeof(value)); // occupancy
    s.write((const char*) &dynamicity, sizeof(dynamicity)); // dynamicity

    return s;
  }

  std::istream& DynamicOcTreeNode::readData(std::istream &s) {
    s.read((char*) &value, sizeof(value)); // occupancy
    s.read((char*) &dynamicity, sizeof(dynamicity)); // dynamicity

    return s;
  }

  float DynamicOcTreeNode::getAverageChildDynamicity() const {
    float mdynamicity = 0;
    int c = 0;

    if (children != NULL){
      for (int i=0; i<8; i++) {
        DynamicOcTreeNode* child = static_cast<DynamicOcTreeNode*>(children[i]);

        if (child != NULL && child->isDynamicitySet()) {
          mdynamicity += child->getDynamicity();
          ++c;
        }
      }
    }

    if (c > 0) {
      mdynamicity /= c;
      return mdynamicity;
    }
    else { // no child had the dynamicity set
      return 0;
    }
  }


  void DynamicOcTreeNode::updateDynamicityChildren() {
    dynamicity = getAverageChildDynamicity();
  }

  // tree implementation  --------------------------------------
  DynamicOcTree::DynamicOcTree(double in_resolution)
   : OccupancyOcTreeBase<DynamicOcTreeNode>(in_resolution) {
    dynamicityOcTreeMemberInit.ensureLinking();
  }

  DynamicOcTreeNode* DynamicOcTree::setNodeDynamicity(const OcTreeKey& key,
                                             float d) {
    DynamicOcTreeNode* n = search (key);
    if (n != 0) {
      n->setDynamicity(d);
    }
    return n;
  }

  bool DynamicOcTree::pruneNode(DynamicOcTreeNode* node) {
    if (!isNodeCollapsible(node))
      return false;

    // set value to children's values (all assumed equal)
    node->copyData(*(getNodeChild(node, 0)));

    if (node->isDynamicitySet()) // TODO check
      node->setDynamicity(node->getAverageChildDynamicity());

    // delete children
    for (unsigned int i=0;i<8;i++) {
      deleteNodeChild(node, i);
    }
    delete[] node->children;
    node->children = NULL;

    return true;
  }

  bool DynamicOcTree::isNodeCollapsible(const DynamicOcTreeNode* node) const{
    // all children must exist, must not have children of
    // their own and have the same occupancy probability
    if (!nodeChildExists(node, 0))
      return false;

    const DynamicOcTreeNode* firstChild = getNodeChild(node, 0);
    if (nodeHasChildren(firstChild))
      return false;

    for (unsigned int i = 1; i<8; i++) {
      // compare nodes only using their occupancy, ignoring dynamicity for pruning
      if (!nodeChildExists(node, i) || nodeHasChildren(getNodeChild(node, i)) || !(getNodeChild(node, i)->getValue() == firstChild->getValue()))
        return false;
    }

    return true;
  }

  DynamicOcTreeNode* DynamicOcTree::incrementNodeDynamicity(const OcTreeKey& key,
                                                 float d) {
    DynamicOcTreeNode* n = search(key);
    if (n != 0) {
      if (n->isDynamicitySet()) {
        float prev_dynamicity = n->getDynamicity();
        if (prev_dynamicity < 1000) {
          n->setDynamicity((prev_dynamicity + d));
        }
      }
      else {
        n->setDynamicity(d);
      }
    }
    return n;
  }

  DynamicOcTreeNode* DynamicOcTree::integrateNodeDynamicity(const OcTreeKey& key,
                                                   float d) {
    DynamicOcTreeNode* n = search (key);
    if (n != 0) {
      if (n->isDynamicitySet()) {
        float prev_dynamicity = n->getDynamicity();
        double node_prob = n->getOccupancy();
        float new_dynamicity = ((double) prev_dynamicity * node_prob
                           +  (double) d * (0.99-node_prob));
        n->setDynamicity(new_dynamicity);
      }
      else {
        n->setDynamicity(d);
      }
    }
    return n;
  }


  void DynamicOcTree::updateInnerOccupancy() {
    this->updateInnerOccupancyRecurs(this->root, 0);
  }

  void DynamicOcTree::updateInnerOccupancyRecurs(DynamicOcTreeNode* node, unsigned int depth) {
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
      node->updateDynamicityChildren();
    }
  }

  DynamicOcTree::StaticMemberInitializer DynamicOcTree::dynamicityOcTreeMemberInit;

} // end namespace
