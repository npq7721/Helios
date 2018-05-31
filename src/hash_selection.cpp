/*
 * hash_selection.cpp
 *
 *  Created on: May 11, 2018
 *      Author: tri
 */

#include "hash_selection.h"

std::vector<std::vector<int>> INITIAL_GROUP = {
		{1,3,2,3,3},
		{4,5,5,6,5},
		{7,7,8,7,9},
		{10,11,10,10,12},
		{13, 14, 14, 15, 14},
		{15, 15, 1, 15, 2},
		{2,2,3,4,2},
		{4,5,6,4,4},
		{6,6,6,7,8},
		{9,10,8,8,8},
		{11,10,10,10,12},
		{12,13,12,14,12},
		{15,14,0,14,14},
		{0,0,1,2,0},
		{0,6,5,0,0},
		{7,2,0,0,0}
};

int HashSelection::getHashSelection(int index) {
	assert(index >= 0);
	assert(index < size);
	const int startNibblesHash = 64 - size;
	int hashSelection = PrevBlockHash.GetNibble(startNibblesHash + index);
	hashSelection = hashSelection % size;
	return(hashSelection);
}

int HashSelection::getGroupHashSelection(uint256 blockHash) {
	return blockHash.GetNibble(60);
}

std::string HashSelection::getHashSelectionString() {
	std::string selectedAlgoes;
	for(int i = 0; i < size; i++) {
		//int hashSelection = getHashSelection(i);
		int selectedAlgoIndex = this->algoGroups[i];
		std::string selectedAlgo = this->algoMap[selectedAlgoIndex];
		selectedAlgoes.append(selectedAlgo);
	}

	return selectedAlgoes;
}





