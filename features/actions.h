#ifndef ACTIONS_H
#define ACTIONS_H

namespace features {

class ActionData;

class Action {
public:
    virtual void Apply() = 0;
    virtual void Revert() = 0;
    virtual void SetActionData(ActionData&& data) = 0;
};

class ActionBuffer {

};

class Insert : public Action {

};

class Delete : public Action {

};

} // namespace features

#endif // ACTIONS_H
