#include "rec_dir_it.hpp"
#include <sys/types.h>
#include <sys/stat.h>
#include <dirent.h>
#include <fcntl.h>
#include <iostream>
#include <fstream>

#include <algorithm>
#include <filesystem>
namespace stdlike {

directory_entry::directory_entry(std::string path) : path_(path) {
    lstat(path.c_str(), &description);
}

std::string directory_entry::path() const {
    return path_;
};

bool directory_entry::is_directory() const {
    return S_ISDIR(description.st_mode);
}

bool directory_entry::is_symlink() const {
    return S_ISLNK(description.st_mode);
}

bool directory_entry::is_block_file() const {
    return S_ISBLK(description.st_mode);
}

bool directory_entry::is_character_file() const {
    return S_ISCHR(description.st_mode);
}

bool directory_entry::is_regular_file() const {
    return S_ISREG(description.st_mode);
}

bool directory_entry::is_fifo() const {
    return S_ISFIFO(description.st_mode);
}

bool directory_entry::is_socket() const {
    return S_ISSOCK(description.st_mode);
}

off_t directory_entry::file_size() const {
    return description.st_size;
}

nlink_t directory_entry::hard_link_count() const {
    return description.st_nlink;
}

timespec directory_entry::last_write_type() const {
    return description.st_mtim;
}

mode_t directory_entry::permissions() const {
    return description.st_mode;
}

recursive_directory_iterator::recursive_directory_iterator(const char* path) : path(path) {
    DIR* start = opendir(path);
    all_dir_pointers.push(start);
    operator++();
}

recursive_directory_iterator::recursive_directory_iterator(const char* path,
                                                           enum directory_options options)
    : options(options), cur_directory(path), path(path) {
    DIR* start = opendir(path);
    all_dir_pointers.push(start);
    operator++();
}

recursive_directory_iterator::recursive_directory_iterator(std::string path, bool is_end)
    : cur_directory(path.c_str()), path(path), is_end(is_end) {
}

recursive_directory_iterator::~recursive_directory_iterator() {
    while (!all_dir_pointers.empty()) {
        if (!is_copied) {
            closedir(all_dir_pointers.top());
        }
        all_dir_pointers.pop();
    }
}

recursive_directory_iterator::recursive_directory_iterator(
    const recursive_directory_iterator& other)
    : cur_directory(other.cur_directory) {
    options = other.options;
    path = other.path;
    all_dir_pointers = other.all_dir_pointers;
    depth_ = other.depth_;
    is_end = other.is_end;
    other.is_copied = true;
}
bool recursive_directory_iterator::operator==(const recursive_directory_iterator& other) const {
    return is_end == other.is_end;
}
bool recursive_directory_iterator::operator!=(const recursive_directory_iterator& other) const {
    return is_end != other.is_end;
}

recursive_directory_iterator begin(recursive_directory_iterator& other) {
    return other;
}

recursive_directory_iterator end(recursive_directory_iterator& other) {
    return recursive_directory_iterator(other.get_path(), true);
}

recursive_directory_iterator& recursive_directory_iterator::operator++() {
    DIR* cur_dir = all_dir_pointers.top();
    struct dirent* cur_object = readdir(cur_dir);
    while (cur_object != nullptr &&
           (std::string(cur_object->d_name) == "." || std::string(cur_object->d_name) == "..")) {
        cur_object = readdir(cur_dir);
    }
    while (cur_object == nullptr && depth_ >= 0) {
        closedir(all_dir_pointers.top());
        all_dir_pointers.pop();
        --depth_;
        if (all_dir_pointers.empty()) {
            cur_dir = nullptr;
        } else {
            cur_dir = all_dir_pointers.top();
        }
        if (cur_dir != nullptr) {
            cur_object = readdir(cur_dir);
            decreasePath();
        }
    }
    while (cur_object != nullptr &&
           (std::string(cur_object->d_name) == "." || std::string(cur_object->d_name) == "..")) {
        cur_object = readdir(cur_dir);
    }
    if (cur_object == nullptr) {
        is_end = true;
        return *this;
    }
    path += "/";
    path += cur_object->d_name;
    cur_directory = directory_entry(path);
    if ((options == directory_options::follow_directory_symlink && cur_directory.is_symlink()) ||
        (cur_object->d_type == DT_DIR && std::string(cur_object->d_name) != "." &&
         std::string(cur_object->d_name) != "..")) {
        if (DIR* new_dir = opendir(path.c_str())) {
            all_dir_pointers.push(new_dir);
            depth_++;
        } else {
            decreasePath();
        }
    } else {
        decreasePath();
    }
    return *this;
}

int recursive_directory_iterator::depth() const {
    return depth_;
}

const directory_entry& recursive_directory_iterator::operator*() const {
    return cur_directory;
}

const directory_entry* recursive_directory_iterator::operator->() const {
    return &cur_directory;
}

void recursive_directory_iterator::decreasePath() {
    int last_occur = path.rfind("/");
    path.resize(last_occur);
}
void recursive_directory_iterator::pop() {
    closedir(all_dir_pointers.top());
    decreasePath();
    all_dir_pointers.pop();
    depth_--;
    cur_directory = directory_entry(path.c_str());
}

}  // namespace stdlike