// Copyright (c) Facebook, Inc. and its affiliates. All Rights Reserved.
#ifndef BUILDING_NODE_EXTENSION
#define BUILDING_NODE_EXTENSION
#endif

#include <node.h>
#include <v8.h>
#include "db/_wrapper.h"

void InitAll(Handle<Object> exports) {
  DBWrapper::Init(exports);
}

NODE_MODULE(rdb, InitAll)
