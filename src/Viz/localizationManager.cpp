#include "localizationManager.h"


localizationManager::localizationManager(){

}
//--------------------------------------------------------------
localizationManager::~localizationManager(){
		data.clear();
}

//----------------------------------------------
void localizationManager::setup(string filePath){

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
		voxel temp;
		
		string line	= buffer.getNextLine();							// get next line
		vector<string> index =	ofSplitString(line, "\t");
		temp.index = ofToInt(index[0]);
		
		temp.words = ofSplitString(index[1], " ");		// break the line into items
		data.push_back(temp);
	}
	//return data;
}

//----------------------------------------------
bool localizationManager::getLabels(int _index, vector<string> _words){
	
//	cout << "datasize"<< data.size();
	for(int i=0; i<data.size(); i++){
//			cout << "in"<<data[i].index;
		if (data[i].index == _index){
//			cout << "found";
			_words= data[i].words;
			return true;
		}
	}
	
	return false;
	
	//for(vector<ofxUIWidget *>::iterator it = embeddedWidgets.begin(); it != embeddedWidgets.end(); ++it)
}

//----------------------------------------------
vector<int> localizationManager::getIndices(int index){
}

//----------------------------------------------
void localizationManager::resetLoadVars(){
	data.clear();
	data.reserve(1200);
}









