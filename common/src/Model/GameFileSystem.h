/*
 Copyright (C) 2010-2017 Kristian Duske

 This file is part of TrenchBroom.

 TrenchBroom is free software: you can redistribute it and/or modify
 it under the terms of the GNU General Public License as published by
 the Free Software Foundation, either version 3 of the License, or
 (at your option) any later version.

 TrenchBroom is distributed in the hope that it will be useful,
 but WITHOUT ANY WARRANTY; without even the implied warranty of
 MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 GNU General Public License for more details.

 You should have received a copy of the GNU General Public License
 along with TrenchBroom. If not, see <http://www.gnu.org/licenses/>.
 */

#ifndef TRENCHBROOM_GAMEFILESYSTEM_H
#define TRENCHBROOM_GAMEFILESYSTEM_H

#include "IO/FileSystem.h"

#include <vector>

namespace TrenchBroom {
    class Logger;

    namespace IO {
        class FileSystem;
        class Path;
    }

    namespace Model {
        class GameConfig;

        class GameFileSystem : public IO::FileSystem {
        public:
            void initialize(const GameConfig& config, const IO::Path& gamePath, const std::vector<IO::Path>& additionalSearchPaths, Logger* logger);
        private:
            void addDefaultAssetPath(const GameConfig& config, Logger* logger);
            void addGameFileSystems(const GameConfig& config, const IO::Path& gamePath, const std::vector<IO::Path>& additionalSearchPaths, Logger* logger);
            void addShaderFileSystem(const GameConfig& config, Logger* logger);
            void addFileSystemPath(const IO::Path& path, Logger* logger);
            void addFileSystemPackages(const GameConfig& config, const IO::Path& searchPath, Logger* logger);
        private:
            bool doDirectoryExists(const IO::Path& path) const override;
            bool doFileExists(const IO::Path& path) const override;
            IO::Path::List doGetDirectoryContents(const IO::Path& path) const override;
            const IO::MappedFile::Ptr doOpenFile(const IO::Path& path) const override;
        };
    }
}

#endif //TRENCHBROOM_GAMEFILESYSTEM_H
