#ifndef RequestTypes_h
#define RequestTypes_h
class RequestTypes{
public:
  enum Code{
    doesNodeExist = 0,
    nodeExists = 1,
    sendMeNodeData = 2,
    sendingNodeData = 3
  };
};
#endif
