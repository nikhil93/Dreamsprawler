#pragma once
#include "ofMain.h"

/**********************************************/
/*			The Talairach Atlas


 Talairach Atlas brain (170mm, 210mm, 200mm)
 1x1x1 mm volume elements within a standardized Talairach space

 The assigned label is hierarchical, and is composed of five levels:
 hemisphere, lobe, gyrus, tissue type, and cell type.
 http://www.talairach.org/labels.txt
 
 from 0 to 1105
 
 */
 


class localizationManager{
public:
	
	localizationManager();
	~localizationManager();
	
	void setup(string file);

	bool getLabels(int index, vector<string> _words);
	vector<int> getIndices(int index);
	//----------------------------------------------
	string getHemisphere(int index);
	string getLobe(int index);
	string getGyrus(int index);
	string getTissueType(int index);
	string getCellType(int index);

	void resetLoadVars();
	
protected:
private:
	
	struct voxel{
		int		index;
		vector <string>	words;
	};
	
	string myFilePath;
	vector<voxel> data;

};