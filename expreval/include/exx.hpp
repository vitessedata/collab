#pragma once

#include <functional>
#include <stdexcept>
#include <string>
#include <string_view>

class bad_sql : public std::runtime_error {
public:
  bad_sql(const std::string &msg);
  virtual ~bad_sql() noexcept {};
};

class exx_error : public std::runtime_error {
public:
  exx_error(const char *file, int line, const std::string &msg);
  virtual ~exx_error() noexcept {};
  const char *where() const { return m_fline.c_str(); }

private:
  std::string m_fline;
};

void exx_init(const std::vector<std::string> &sssd_path);
void exx_final();

struct xrg_vector_t;

// This function is called to emit results.
// There are nitem rows per column.
// The columns are in value[nvalue].
using emitfn_t = std::function<void(const int nitem, const int nvalue,
                                    xrg_vector_t **value)>;

// This function takes an array of tablenames, and returns
// 1. a string containing the schema in json
// 2. a list of xrg file paths to scan
using datasourcefn_t =
    std::function<std::pair<std::string, std::vector<std::string>>(
        const std::vector<std::string> &tablename)>;

using emitplanfn_t = std::function<void(const std::string &plan)>;

void exx_run(const std::string &sql, emitfn_t emitfn,
             datasourcefn_t datasourcefn, emitplanfn_t emitplan);
