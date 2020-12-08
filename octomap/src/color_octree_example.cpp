// trunc8 did this

// #include <iostream>
#include <octomap/octomap.h>
#include <octomap/ColorOcTree.h>

using namespace std;
using namespace octomap;

void print_query_info(point3d query, OcTreeNode* node) {
  if (node != NULL) {
    cout << "occupancy probability at " << query << ":\t " << node->getOccupancy() << endl;
  }
  else 
    cout << "occupancy probability at " << query << ":\t is unknown" << endl;    
}

int main(int argc, char** argv) {
	cout << "Hello Octomap World\n\n";

  ColorOcTree* tree = new ColorOcTree(0.1);  // create empty tree with resolution 0.1
  cout << "Tree Functions:\n";
  cout << "Tree type: " << tree->getTreeType() << endl;

  float x = 10.0f;
  point3d endpoint (x,x,x);
  tree-> updateNode(endpoint, true);
  tree-> updateNode(endpoint, true);
  ColorOcTreeNode* node2 = tree->setNodeColor(x,x,x,50,50,50);
  cout << "Node2 color: " << node2->getColor() << endl;
  cout << "Node2 occupancy: " << node2->getOccupancy() << endl;




  ColorOcTreeNode* node = new ColorOcTreeNode();
  node->setColor(122,122,122);



  cout << "\nNode Functions:\n";
  cout << "Average child color: " << node->getAverageChildColor() << endl;
  cout << "Node color: " << node->getColor() << endl;
  node->updateColorChildren();
  cout << "Updated node color: " << node->getColor() << endl;
  cout << "Node occupancy: " << node->getOccupancy() << endl;


  cout << "\nGenerating example map\n";
  // insert some measurements of occupied cells

  for (int x=-20; x<20; x++) {
    for (int y=-20; y<20; y++) {
      for (int z=-20; z<20; z++) {
        point3d endpoint ((float) x*0.05f, (float) y*0.05f, (float) z*0.05f);
        tree->updateNode(endpoint, true); // integrate 'occupied' measurement
      }
    }
  }

  // insert some measurements of free cells

  for (int x=-30; x<30; x++) {
    for (int y=-30; y<30; y++) {
      for (int z=-30; z<30; z++) {
        point3d endpoint ((float) x*0.02f-1.0f, (float) y*0.02f-1.0f, (float) z*0.02f-1.0f);
        tree->updateNode(endpoint, false);  // integrate 'free' measurement
      }
    }
  }

  cout << endl;
  cout << "performing some queries:" << endl;
  
  point3d query (0.5, 0.5, 0.5);
  OcTreeNode* result = tree->search (query);
  print_query_info(query, result);

  query = point3d(-1.,-1.,-1.);
  result = tree->search (query);
  print_query_info(query, result);

  query = point3d(1.,1.,1.);
  result = tree->search (query);
  print_query_info(query, result);

  tree->writeColorHistogram("hIsToGrAm");


	return 0;
}