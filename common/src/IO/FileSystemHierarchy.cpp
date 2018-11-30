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

#include "FileSystemHierarchy.h"

#include "CollectionUtils.h"
#include "StringUtils.h"
#include "IO/DiskFileSystem.h"
#include "IO/FileMatcher.h"
#include "IO/IdPakFileSystem.h"

namespace TrenchBroom {
    namespace IO {
        FileSystemHierarchy::FileSystemHierarchy() {}

        FileSystemHierarchy::~FileSystemHierarchy() {
            clear();
        }
        
        void FileSystemHierarchy::pushFileSystem(FileSystem* fileSystem) {
            ensure(fileSystem != nullptr, "filesystem is null");
            m_fileSystems.push_back(fileSystem);
        }

        void FileSystemHierarchy::popFileSystem() {
            ensure(!m_fileSystems.empty(), "filesystem hierarchy is empty");
            delete m_fileSystems.back();
            m_fileSystems.pop_back();
        }

        void FileSystemHierarchy::clear() {
            VectorUtils::clearAndDelete(m_fileSystems);
        }

        Path FileSystemHierarchy::doMakeAbsolute(const Path& relPath) const {
            const auto* fileSystem = findFileSystemContaining(relPath);
            if (fileSystem != nullptr) {
                return fileSystem->makeAbsolute(relPath);
            } else {
                return Path("");
            }
        }

        bool FileSystemHierarchy::doDirectoryExists(const Path& path) const {
            for (auto it = m_fileSystems.rbegin(), end = m_fileSystems.rend(); it != end; ++it) {
                const auto* fileSystem = *it;
                if (fileSystem->directoryExists(path)) {
                    return true;
                }
            }
            return false;
        }
        
        bool FileSystemHierarchy::doFileExists(const Path& path) const {
            return (findFileSystemContaining(path)) != nullptr;
        }
        
        FileSystem* FileSystemHierarchy::findFileSystemContaining(const Path& path) const {
            for (auto it = m_fileSystems.rbegin(), end = m_fileSystems.rend(); it != end; ++it) {
                auto* fileSystem = *it;
                if (fileSystem->fileExists(path)) {
                    return fileSystem;
                }
            }
            return nullptr;
        }

        Path::List FileSystemHierarchy::doGetDirectoryContents(const Path& path) const {
            Path::List result;
            for (auto it = m_fileSystems.rbegin(), end = m_fileSystems.rend(); it != end; ++it) {
                const auto* fileSystem = *it;
                if (fileSystem->directoryExists(path)) {
                    const Path::List contents = fileSystem->getDirectoryContents(path);
                    VectorUtils::append(result, contents);
                }
            }
            
            VectorUtils::sortAndRemoveDuplicates(result);
            return result;
        }
        
        const MappedFile::Ptr FileSystemHierarchy::doOpenFile(const Path& path) const {
            const auto resolvedPath = resolve(path);

            for (auto it = m_fileSystems.rbegin(), end = m_fileSystems.rend(); it != end; ++it) {
                const FileSystem* fileSystem = *it;
                if (fileSystem->fileExists(resolvedPath)) {
                    const auto file = fileSystem->openFile(resolvedPath);
                    if (file.get() != nullptr) {
                        // Links should be transparent, so we set the original file path instead of the resolved path.
                        file->setPath(makeAbsolute(path));
                        return file;
                    }
                }
            }
            return MappedFile::Ptr();
        }

        Path FileSystemHierarchy::doResolve(const Path& path) const {
            for (auto it = m_fileSystems.rbegin(), end = m_fileSystems.rend(); it != end; ++it) {
                const FileSystem* fileSystem = *it;
                if (fileSystem->fileExists(path)) {
                    return fileSystem->resolve(path);
                }
            }
            return path;
        }

        WritableFileSystemHierarchy::WritableFileSystemHierarchy() :
        m_writableFileSystem(nullptr) {}
        
        void WritableFileSystemHierarchy::pushReadableFileSystem(FileSystem* fileSystem) {
            pushFileSystem(fileSystem);
        }
        
        void WritableFileSystemHierarchy::pushWritableFileSystem(WritableFileSystem* fileSystem) {
            assert(m_writableFileSystem == nullptr);
            pushFileSystem(fileSystem);
            m_writableFileSystem = fileSystem;
        }
        
        void WritableFileSystemHierarchy::clear() {
            FileSystemHierarchy::clear();
            m_writableFileSystem = nullptr;
        }

        void WritableFileSystemHierarchy::doCreateFile(const Path& path, const String& contents) {
            ensure(m_writableFileSystem != nullptr, "writableFileSystem is null");
            m_writableFileSystem->createFile(path, contents);
        }

        void WritableFileSystemHierarchy::doCreateDirectory(const Path& path) {
            ensure(m_writableFileSystem != nullptr, "writableFileSystem is null");
            m_writableFileSystem->createDirectory(path);
        }
        
        void WritableFileSystemHierarchy::doDeleteFile(const Path& path) {
            ensure(m_writableFileSystem != nullptr, "writableFileSystem is null");
            m_writableFileSystem->deleteFile(path);
        }
        
        void WritableFileSystemHierarchy::doCopyFile(const Path& sourcePath, const Path& destPath, const bool overwrite) {
            ensure(m_writableFileSystem != nullptr, "writableFileSystem is null");
            m_writableFileSystem->copyFile(sourcePath, destPath, overwrite);
        }
        
        void WritableFileSystemHierarchy::doMoveFile(const Path& sourcePath, const Path& destPath, const bool overwrite) {
            ensure(m_writableFileSystem != nullptr, "writableFileSystem is null");
            m_writableFileSystem->moveFile(sourcePath, destPath, overwrite);
        }
    }
}
