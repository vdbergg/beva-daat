//
// Created by berg on 28/01/19.
//

#include <queue>
#include <math.h>
#include <bitset>
#include "EditVectorAutomaton.h"
#include "utils.h"

EditVectorAutomaton::EditVectorAutomaton(int editDistanceThreshold) {
    this->editDistanceThreshold = editDistanceThreshold;
    this->initialState = nullptr;
    this->finalState = nullptr;
    this->size = 0;
}

EditVectorAutomaton::~EditVectorAutomaton() = default;

State* EditVectorAutomaton::setTransition(State* state, string bitmap) {
    EditVector* editVector = new EditVector(this->editDistanceThreshold, state->editVector);
    editVector->buildEditVectorWithBitmap(bitmap);

    State* newState = new State(editVector, this->size);
    state->transitions[bitmap] = newState;

    return newState;
}

string getVetValue(State* state, int editDistanceThreshold) {
    string vetValue = "";
    int* vet = state->editVector->vector;

    for (int i = 0; i < state->editVector->size; i++) {
        vetValue += vet[i] > editDistanceThreshold ? "#" : to_string(vet[i]);
    }

    return vetValue;
}

void EditVectorAutomaton::buildAutomaton() {
    map<string, int> states;

    EditVector* editVector = new EditVector(this->editDistanceThreshold, nullptr);
    editVector->buildInitialEditVector();
    this->initialState = new State(editVector, this->size);

    string editVectorValue = getVetValue(this->initialState, this->editDistanceThreshold);
    states[editVectorValue] = this->initialState->id;
    this->size++;

    queue<State*> queue;
    queue.push(this->initialState);

    int bitmapSize = (2 * this->editDistanceThreshold) + 1;

    while (!queue.empty()) {
        State* state = queue.front();
        queue.pop();

        int countTransitions = (int) pow(2, bitmapSize);
        int count = 0;

        while (count < countTransitions) {
            string bitmap = bitset<16>(count).to_string();
            bitmap = bitmap.substr(bitmap.length() - bitmapSize);

            State* newState = this->setTransition(state, bitmap);
            editVectorValue = getVetValue(newState, this->editDistanceThreshold);

            if (states.find(editVectorValue) == states.end()) { // if not exists state in automaton
                states[editVectorValue] = newState->id;
                this->size++;
                queue.push(newState);

                if (newState->isFinal) {
                    this->finalState = newState;
                }
            }

            count++;
        }
    }
}
