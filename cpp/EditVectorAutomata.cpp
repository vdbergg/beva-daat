//
// Created by berg on 28/01/19.
//

#include <queue>
#include <math.h>
#include <bitset>
#include "../header/EditVectorAutomata.h"
#include "../header/utils.h"
#include <utility>
#include <unordered_map>

EditVectorAutomata::EditVectorAutomata(int editDistanceThreshold) {
    this->editDistanceThreshold = editDistanceThreshold;
    this->initialState = nullptr;
    this->finalState = nullptr;
    this->size = 0;
}

EditVectorAutomata::~EditVectorAutomata() {
    delete this->initialState;
    delete this->finalState;
};

State* EditVectorAutomata::setTransition(State*& state, string& bitmap, string& initialStateValue,
        unordered_map<string, State*>& states) {
    EditVector* editVector = new EditVector(this->editDistanceThreshold, state->editVector);
    editVector->buildEditVectorWithBitmap(bitmap, initialStateValue);

    State* newState = nullptr;
    if (state->editVector->value == editVector->value) {
    } else if (states.find(editVector->value) == states.end()) { // if not exists state in automaton
        newState = new State(editVector, this->size);
    } else {
        newState = states[editVector->value];
    }
    state->transitions[bitmap] = newState;
    return newState;
}

void EditVectorAutomata::buildAutomaton() {
    unordered_map<string, State*> states;

    EditVector* editVector = new EditVector(this->editDistanceThreshold, nullptr);
    editVector->buildInitialEditVector();
    this->initialState = new State(editVector, this->size);

    states[editVector->value] = this->initialState;
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

            State* newState = this->setTransition(state, bitmap, this->initialState->editVector->value, states);

            if (newState != nullptr) { // if not exists state in automaton
                states[newState->editVector->value] = newState;
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
