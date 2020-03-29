//
// Created by berg on 28/01/19.
//

#include <queue>
#include "../header/EditVectorAutomata.h"
#include <unordered_map>

EditVectorAutomata::EditVectorAutomata(int editDistanceThreshold) {
    this->editDistanceThreshold = editDistanceThreshold;
    this->initialState = nullptr;
    this->finalState = nullptr;
    this->size = 0;
    this->transitionsByState = (1 << ((2 * this->editDistanceThreshold) + 1)); // 2^2tau + 1
}

EditVectorAutomata::~EditVectorAutomata() {
    delete this->initialState;
    delete this->finalState;
};

bool checkToTerminalEditVector(EditVector* editVector, int editDistanceThreshold) {
    bool isFinal = true;
    for (int i = 0; i < editVector->size; i++) {
        if (editVector->vector[i] <= editDistanceThreshold) {
            isFinal = false;
            break;
        }
    }
    return isFinal;
}

State* EditVectorAutomata::setTransition(State*& state, unsigned bitmap, unordered_map<string, State*>& states) {
    EditVector* editVector = new EditVector(this->editDistanceThreshold, state->editVector);
    editVector->buildEditVectorWithBitmap(bitmap);
    string signature = editVector->getEditVectorSignature();

    State* newState = nullptr;
    if (state->editVector->getEditVectorSignature() == signature) { // State already exists, by convention we defined null when an state point to yourself
    } else if (states.find(signature) == states.end()) { // if not exists state in automaton
        bool isFinal = checkToTerminalEditVector(editVector, this->editDistanceThreshold);
        newState = new State(editVector, this->size, false, isFinal);
    } else {
        newState = states[signature];
        state->transitions[bitmap] = newState;
        return nullptr;
    }
    state->transitions[bitmap] = newState;
    return newState;
}

void EditVectorAutomata::buildAutomaton() {
    unordered_map<string, State*> states;

    EditVector* editVector = new EditVector(this->editDistanceThreshold, nullptr);
    editVector->buildInitialEditVector();
    this->initialState = new State(editVector, this->size, true);

    states[this->initialState->editVector->getEditVectorSignature()] = this->initialState;
    this->size++;

    queue<State*> queue;
    queue.push(this->initialState);

    while (!queue.empty()) {
        State* state = queue.front();
        queue.pop();

        int countTransitions = this->transitionsByState;
        unsigned count = 0;

        while (count < countTransitions) {
            State* newState = this->setTransition(state, count, states);

            if (newState != nullptr) { // if not exists state in automaton
                states[newState->editVector->getEditVectorSignature()] = newState;
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
