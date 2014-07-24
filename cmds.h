/*
 * cmds.h
 * Copyright (C) 2014 Krystian Dużyński <krystian.duzynski@gmail.com>
 */

#ifndef __CMDS_H__
#define __CMDS_H__

int getVersion();
int getCommand();
int getID();
int readoutUnprotect();
int writeUnprotect();
int readoutProtect();
int erase();

#endif
