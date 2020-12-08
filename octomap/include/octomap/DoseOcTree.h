// trunc8 did this

#ifndef OCTOMAP_DOSE_OCTREE_H
#define OCTOMAP_DOSE_OCTREE_H


#include <octomap/OcTreeNode.h>
#include <octomap/OccupancyOcTreeBase.h>

namespace octomap {

  // forward declaraton for "friend"
  class DoseOcTree;

  // node definition
  class DoseOcTreeNode : public OcTreeNode {

  public:
    friend class DoseOcTree; // needs access to node children (inherited)
    
    DoseOcTreeNode() : OcTreeNode(), dose(0) {}

    DoseOcTreeNode(const DoseOcTreeNode& rhs) : OcTreeNode(rhs), dose(rhs.dose) {}

    bool operator==(const DoseOcTreeNode& rhs) const{
      return (rhs.value == value && rhs.dose == dose);
    }

    void copyData(const DoseOcTreeNode& from){
      OcTreeNode::copyData(from);
      this->dose = from.getDose();
    }

    // dose
    inline float getDose() const { return dose; }
    inline void setDose(float d) { this->dose = d; }

    inline bool isDoseSet() const {
      return (dose != 0);
    }

    void updateDoseChildren();
    float getAverageChildDose() const;

    // file I/O
    std::istream& readData(std::istream &s);
    std::ostream& writeData(std::ostream &s) const;

  protected:
    float dose;
  };


  // tree definition
  class DoseOcTree : public OccupancyOcTreeBase <DoseOcTreeNode> {

  public:
    /// Default constructor, sets resolution of leafs
	  DoseOcTree(double resolution);

    /// virtual constructor: creates a new object of same type
    /// (Covariant return type requires an up-to-date compiler)
    DoseOcTree* create() const {return new DoseOcTree(resolution); }

    std::string getTreeType() const {return "DoseOcTree";}

//// MORE STUFF ADDED FROM COLOROCTREE.H
     /**
     * Prunes a node when it is collapsible. This overloaded
     * version only considers the node occupancy for pruning,
     * different doses of child nodes are ignored.
     * @return true if pruning was successful
     */
    virtual bool pruneNode(DoseOcTreeNode* node);
    
    virtual bool isNodeCollapsible(const DoseOcTreeNode* node) const;

    // set node dose at given key or coordinate. Replaces previous dose.
    DoseOcTreeNode* setNodeDose(const OcTreeKey& key, float d);

    DoseOcTreeNode* setNodeDose(float x, float y, 
                                 float z, float d) {
      OcTreeKey key;
      if (!this->coordToKeyChecked(point3d(x,y,z), key)) return NULL;
      return setNodeDose(key,d);
    }

    // increment dose measurement at given key or coordinate
    DoseOcTreeNode* incrementNodeDose(const OcTreeKey& key, float d);
    
    DoseOcTreeNode* incrementNodeDose(float x, float y, 
                                      float z, float d) {
      OcTreeKey key;
      if (!this->coordToKeyChecked(point3d(x,y,z), key)) return NULL;
      return incrementNodeDose(key,d);
    }

    // integrate dose measurement at given key or coordinate. Average with previous dose 
    // based on occupancy probability
    DoseOcTreeNode* integrateNodeDose(const OcTreeKey& key, float d);
    
    DoseOcTreeNode* integrateNodeDose(float x, float y, 
                                      float z, float d) {
      OcTreeKey key;
      if (!this->coordToKeyChecked(point3d(x,y,z), key)) return NULL;
      return integrateNodeDose(key,d);
    }

    // update inner nodes, sets dose to average child dose
    void updateInnerOccupancy();

  protected:
    void updateInnerOccupancyRecurs(DoseOcTreeNode* node, unsigned int depth);
//// END OF ADDED STUFF FROM COLOROCTREE.H

    /**
     * Static member object which ensures that this OcTree's prototype
     * ends up in the classIDMapping only once. You need this as a
     * static member in any derived octree class in order to read .ot
     * files through the AbstractOcTree factory. You should also call
     * ensureLinking() once from the constructor.
     */
    class StaticMemberInitializer{
    public:
      StaticMemberInitializer() {
        DoseOcTree* tree = new DoseOcTree(0.1);
        tree->clearKeyRays();
        AbstractOcTree::registerTreeType(tree);
      }

      /**
      * Dummy function to ensure that MSVC does not drop the
      * StaticMemberInitializer, causing this tree failing to register.
      * Needs to be called from the constructor of this octree.
      */
      void ensureLinking() {};
    };
    /// to ensure static initialization (only once)
    static StaticMemberInitializer doseOcTreeMemberInit;

  };

} // end namespace

#endif
