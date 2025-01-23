#pragma once
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <iostream>
#include <stack>
#include <dirent.h>
#include <string>
#include <set>
namespace stdlike {

enum class directory_options { none, follow_directory_symlink, skip_permission_denied };

class directory_entry {
public:
    directory_entry() = default;
    directory_entry(std::string path);
    directory_entry(const directory_entry& other) = default;
    directory_entry& operator=(const directory_entry& other) = default;
    std::string path() const;
    bool is_directory() const;
    bool is_symlink() const;
    bool is_regular_file() const;
    bool is_block_file() const;
    bool is_character_file() const;
    bool is_socket() const;
    bool is_fifo() const;
    off_t file_size() const;
    nlink_t hard_link_count() const;
    timespec last_write_type() const;
    mode_t permissions() const;

private:
    std::string path_;
    struct stat description;
};

class recursive_directory_iterator {
public:
    explicit recursive_directory_iterator(const char* path);
    recursive_directory_iterator(const char* path, enum directory_options options);
    recursive_directory_iterator(std::string path, bool is_end);
    ~recursive_directory_iterator();
    recursive_directory_iterator(const recursive_directory_iterator& other);
    bool operator==(const recursive_directory_iterator& other) const;
    bool operator!=(const recursive_directory_iterator& other) const;
    recursive_directory_iterator& operator++();
    int depth() const;
    void pop();
    const directory_entry& operator*() const;
    const directory_entry* operator->() const;
    std::string get_path() {
        return path;
    }

private:
    void decreasePath();
    enum directory_options options = directory_options::none;
    directory_entry cur_directory;
    std::string path;
    std::stack<DIR*> all_dir_pointers;
    int depth_ = 0;
    bool is_end = false;
    mutable bool is_copied = false;
};

recursive_directory_iterator end(recursive_directory_iterator& other);
recursive_directory_iterator begin(recursive_directory_iterator& other);

}  // namespace stdlike