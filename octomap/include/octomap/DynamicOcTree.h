// trunc8 did this

#ifndef OCTOMAP_DYNAMIC_OCTREE_H
#define OCTOMAP_DYNAMIC_OCTREE_H


#include <octomap/OcTreeNode.h>
#include <octomap/OccupancyOcTreeBase.h>

namespace octomap {

  // forward declaraton for "friend"
  class DynamicOcTree;

  // node definition
  class DynamicOcTreeNode : public OcTreeNode {

  public:
    friend class DynamicOcTree; // needs access to node children (inherited)
    
    DynamicOcTreeNode() : OcTreeNode(), dynamicity(0) {}

    DynamicOcTreeNode(const DynamicOcTreeNode& rhs) : OcTreeNode(rhs), dynamicity(rhs.dynamicity) {}

    bool operator==(const DynamicOcTreeNode& rhs) const{
      return (rhs.value == value && rhs.dynamicity == dynamicity);
    }

    void copyData(const DynamicOcTreeNode& from){
      OcTreeNode::copyData(from);
      this->dynamicity = from.getDynamicity();
    }

    // dynamicity
    inline float getDynamicity() const { return dynamicity; }
    inline void setDynamicity(float d) { this->dynamicity = d; }

    inline bool isDynamicitySet() const {
      return (dynamicity != 0);
    }

    void updateDynamicityChildren();
    float getAverageChildDynamicity() const;

    // file I/O
    std::istream& readData(std::istream &s);
    std::ostream& writeData(std::ostream &s) const;

  protected:
    float dynamicity;
  };


  // tree definition
  class DynamicOcTree : public OccupancyOcTreeBase <DynamicOcTreeNode> {

  public:
    /// Default constructor, sets resolution of leafs
    DynamicOcTree(double resolution);

    /// virtual constructor: creates a new object of same type
    /// (Covariant return type requires an up-to-date compiler)
    DynamicOcTree* create() const {return new DynamicOcTree(resolution); }

    std::string getTreeType() const {return "DynamicOcTree";}

//// MORE STUFF ADDED FROM COLOROCTREE.H
     /**
     * Prunes a node when it is collapsible. This overloaded
     * version only considers the node occupancy for pruning,
     * different dynamicities of child nodes are ignored.
     * @return true if pruning was successful
     */
    virtual bool pruneNode(DynamicOcTreeNode* node);
    
    virtual bool isNodeCollapsible(const DynamicOcTreeNode* node) const;

    // set node dynamicity at given key or coordinate. Replaces previous dynamicity.
    DynamicOcTreeNode* setNodeDynamicity(const OcTreeKey& key, float d);

    DynamicOcTreeNode* setNodeDynamicity(float x, float y, 
                                 float z, float d) {
      OcTreeKey key;
      if (!this->coordToKeyChecked(point3d(x,y,z), key)) return NULL;
      return setNodeDynamicity(key,d);
    }

    // increment dynamicity measurement at given key or coordinate
    DynamicOcTreeNode* incrementNodeDynamicity(const OcTreeKey& key, float d);
    
    DynamicOcTreeNode* incrementNodeDynamicity(float x, float y, 
                                      float z, float d) {
      OcTreeKey key;
      if (!this->coordToKeyChecked(point3d(x,y,z), key)) return NULL;
      return incrementNodeDynamicity(key,d);
    }

    // integrate dynamicity measurement at given key or coordinate. Average with previous dynamicity 
    // based on occupancy probability
    DynamicOcTreeNode* integrateNodeDynamicity(const OcTreeKey& key, float d);
    
    DynamicOcTreeNode* integrateNodeDynamicity(float x, float y, 
                                      float z, float d) {
      OcTreeKey key;
      if (!this->coordToKeyChecked(point3d(x,y,z), key)) return NULL;
      return integrateNodeDynamicity(key,d);
    }

    // update inner nodes, sets dynamicity to average child dynamicity
    void updateInnerOccupancy();

  protected:
    void updateInnerOccupancyRecurs(DynamicOcTreeNode* node, unsigned int depth);
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
        DynamicOcTree* tree = new DynamicOcTree(0.1);
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
    static StaticMemberInitializer dynamicityOcTreeMemberInit;

  };

} // end namespace

#endif
