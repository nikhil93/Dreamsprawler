#include "talairachLabels.h"


talairachLabels::talairachLabels(){

}
//--------------------------------------------------------------
talairachLabels::~talairachLabels(){
		data.clear();
}

//----------------------------------------------
void talairachLabels::setup(string filePath){

	ofLogNotice("TaliarchLabels")<< "Loading Talairach Atlas";

	//Reset Load Data Vars
	resetLoadVars();
	
	ofFile file(filePath);
	if(!file.exists()){
		ofLogError("The file " + filePath + " is missing");
	}
	
	ofBuffer buffer(file);
	while (!buffer.isLastLine()) {
		cout << ".";
		TaliarchData temp;
		
		string line	= buffer.getNextLine();							// get next line
		vector<string> index =	ofSplitString(line, "\t");
		temp.index = ofToInt(index[0]);
		
		vector<string> items = ofSplitString(index[1], ".");		// break the line into items
		
		//get labels
		temp.hemisphere = items[0];
		temp.lobe = items[1];
		temp.gyrus = items[2];
		temp.tissueType = items[3];
		temp.cellType = items[4];
		
        data.push_back(temp);
	}
}
//----------------------------------------------
vector<string> talairachLabels::getLabels(int index){
//	return data[index].hemisphere;
}

//----------------------------------------------
vector<int> talairachLabels::getIndices(int index){
}

//----------------------------------------------
string talairachLabels::getHemisphere(int index){
	int i = data[index].index;
	string hemisphere = data[index].hemisphere;
	ofLogVerbose("Talairach")<< "i= "<< i <<" "<< hemisphere;
	return hemisphere;
}

//----------------------------------------------
string talairachLabels::getLobe(int index){
	int i = data[index].index;
	string lobe = data[index].lobe;
	ofLogVerbose("Talairach")<< "i= "<< i <<" "<< lobe;
	return lobe;
}

//----------------------------------------------
string talairachLabels::getGyrus(int index){
	int i = data[index].index;
	string gyrus = data[index].gyrus;
	ofLogVerbose("Talairach")<< "i= "<< i <<" "<< gyrus;
	return gyrus;
}

//----------------------------------------------
string talairachLabels::getTissueType(int index){
	int i = data[index].index;
	string tissueType = data[index].tissueType;
//	ofLogVerbose("Talairach")<< "i= "<< i <<" "<< tissueType;
	return tissueType;
}

//----------------------------------------------
string talairachLabels::getCellType(int index){
	int i = data[index].index;
	string cellType = data[index].cellType;
	ofLogVerbose("Talairach")<< "i= "<< i <<" "<< cellType;
	return cellType;
}

//----------------------------------------------
void talairachLabels::resetLoadVars(){
	data.clear();
	data.reserve(1200);
}
