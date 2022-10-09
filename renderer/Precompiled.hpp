// SPDX-FileCopyrightText: 2022 Admer Šuko
// SPDX-License-Identifier: MIT

#pragma once

#include "common/Precompiled.hpp"

inline ICore* Core;
inline IConsole* Console;
inline IFileSystem* FileSystem;
inline IModelManager* ModelManager;
inline IMaterialManager* MaterialManager;

// Since we're exclusively dealing with render concepts here, I think it's sane to
// pull in everything from the Render namespace
using namespace Render;
