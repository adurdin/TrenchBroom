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

#ifndef TrenchBroom_FileSystem
#define TrenchBroom_FileSystem

#include "CollectionUtils.h"
#include "Macros.h"
#include "Functor.h"
#include "StringUtils.h"
#include "IO/DiskIO.h"
#include "IO/MappedFile.h"
#include "IO/Path.h"

#include <iostream>
#include <memory>

namespace TrenchBroom {
    namespace IO {
        class Path;
        
        class FileSystem {
            deleteCopyAndMove(FileSystem)
        protected:
            std::unique_ptr<FileSystem> m_next;
        public: // public API
            explicit FileSystem(std::unique_ptr<FileSystem> next = std::unique_ptr<FileSystem>());
            virtual ~FileSystem();

            bool hasNext() const;
            const FileSystem& next() const;
            std::unique_ptr<FileSystem> releaseNext();

            bool directoryExists(const Path& path) const;
            bool fileExists(const Path& path) const;

            /**
             * Find all files with the same path and base name as the given path, regardless of the extension.
             *
             * @param path the file path
             * @param extensions a list of extensions to match
             * @return a list of paths
             */
            Path::List findItemsWithBaseName(const Path& path, const StringList& extensions) const;

            template <class Matcher>
            Path::List findItems(const Path& directoryPath, const Matcher& matcher) const {
                Path::List result;
                _findItems(directoryPath, matcher, false, result);
                return result;
            }
            Path::List findItems(const Path& directoryPath) const;
            
            template <class Matcher>
            Path::List findItemsRecursively(const Path& directoryPath, const Matcher& matcher) const {
                Path::List result;
                _findItems(directoryPath, matcher, true, result);
                VectorUtils::sortAndRemoveDuplicates(result);
                return result;
            }
            Path::List findItemsRecursively(const Path& directoryPath) const;
            
            Path::List getDirectoryContents(const Path& directoryPath) const;
            MappedFile::Ptr openFile(const Path& path) const;
        private: // private API to be used for chaining, avoids multiple checks of parameters
            bool _directoryExists(const Path& path) const;
            bool _fileExists(const Path& path) const;
            Path::List _getDirectoryContents(const Path& directoryPath) const;
            MappedFile::Ptr _openFile(const Path& path) const;

            /**
             * Finds all items matching the given matcher at the given search path, optionally recursively, and adds
             * the matches to the given result.
             *
             * Delegates the query to the next file system if one exists.
             *
             * @tparam M the matcher type
             * @param searchPath the search path at which to search for matches
             * @param matcher the matcher to apply to candidates
             * @param recurse whether or not to recurse into sub directories
             * @param result collects the matching paths
             */
            template <class M>
            void _findItems(const Path& searchPath, const M& matcher, const bool recurse, Path::List& result) const {
                doFindItems(searchPath, matcher, recurse, result);
                if (m_next) {
                    m_next->_findItems(searchPath, matcher, recurse, result);
                }
            }

            /**
             * Finds all items matching the given matcher at the given search path, optionally recursively, and adds
             * the matches to the given result.
             *
             * @tparam M the matcher type
             * @param searchPath the search path at which to search for matches
             * @param matcher the matcher to apply to candidates
             * @param recurse whether or not to recurse into sub directories
             * @param result collects the matching paths
             */
            template <class M>
            void doFindItems(const Path& searchPath, const M& matcher, const bool recurse, Path::List& result) const {
                if (doDirectoryExists(searchPath)) {
                    for (const auto& itemPath : doGetDirectoryContents(searchPath)) {
                        const auto directory = doDirectoryExists(searchPath + itemPath);
                        if (directory && recurse) {
                            doFindItems(searchPath + itemPath, matcher, recurse, result);
                        }
                        if (matcher(searchPath + itemPath, directory)) {
                            result.push_back(searchPath + itemPath);
                        }
                    }
                }
            }
        private: // subclassing API
            virtual bool doDirectoryExists(const Path& path) const = 0;
            virtual bool doFileExists(const Path& path) const = 0;
            
            virtual Path::List doGetDirectoryContents(const Path& path) const = 0;

            virtual const MappedFile::Ptr doOpenFile(const Path& path) const = 0;
        };
        
        class WritableFileSystem : public virtual FileSystem {
            deleteCopyAndMove(WritableFileSystem)
        public:
            WritableFileSystem(std::unique_ptr<FileSystem> next = std::unique_ptr<FileSystem>());
            virtual ~WritableFileSystem();

            void createFile(const Path& path, const String& contents);
            void createDirectory(const Path& path);
            void deleteFile(const Path& path);
            void copyFile(const Path& sourcePath, const Path& destPath, bool overwrite);
            void moveFile(const Path& sourcePath, const Path& destPath, bool overwrite);
        private:
            virtual void doCreateFile(const Path& path, const String& contents) = 0;
            virtual void doCreateDirectory(const Path& path) = 0;
            virtual void doDeleteFile(const Path& path) = 0;
            virtual void doCopyFile(const Path& sourcePath, const Path& destPath, bool overwrite) = 0;
            virtual void doMoveFile(const Path& sourcePath, const Path& destPath, bool overwrite) = 0;
        };
    }
}

#endif /* defined(TrenchBroom_FileSystem) */
