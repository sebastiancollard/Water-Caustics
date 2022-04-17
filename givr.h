#pragma once
//------------------------------------------------------------------------------
// Start utility.h
//------------------------------------------------------------------------------
#include <iostream>
#include <tuple>
#include <type_traits>
#include <utility>

namespace givr {
namespace utility {

// named types
template <typename T, typename Parameter> class Type {
public:
  explicit Type() = default;
  explicit Type(T const &value) : m_value(value) {}

  template <typename... Args>
  explicit Type(Args &&... args) : m_value(std::forward<Args>(args)...) {}

  T &value() { return m_value; }
  T const &value() const { return m_value; }
  T &operator*() noexcept { return m_value; }
  operator T() const { return m_value; }

private:
  T m_value;
};

template <typename T, typename Parameter>
std::ostream &operator<<(std::ostream &out, Type<T, Parameter> const &type) {
  return out << type.value();
}

// contains
template <typename T, typename... Ts>
constexpr bool contains = (std::is_same<T, Ts>{} || ...);

// is subset of
template <typename Subset, typename Set> constexpr bool is_subset_of = false;

template <typename... Ts, typename... Us>
constexpr bool is_subset_of<std::tuple<Ts...>, std::tuple<Us...>> =
    (contains<Ts, Us...> && ...);

// has duplicates
template <typename... T1> constexpr bool has_duplicate_types = false;

template <typename T1, typename T2>
constexpr bool has_duplicate_types<T1, T2> = (std::is_same<T1, T2>::value);

template <typename T1, typename T2, typename... Trest>
constexpr bool has_duplicate_types<T1, T2, Trest...> =
    (has_duplicate_types<T1, T2> ||       // test each other
     has_duplicate_types<T1, Trest...> || // test T1 vs. rest
     has_duplicate_types<T2, Trest...>);  // test T2 vs. rest

template <typename... Types> struct TypedStruct {
  using Args = std::tuple<Types...>;
  Args args;

  template <typename T> T const &value() const { return std::get<T>(args); }
  template <typename T> T &value() { return std::get<T>(args); }

  template <typename T> void set(T &&v) { value<T>() = std::forward<T>(v); }
  void set(Args const &a) { args = a; }

  template <typename T, typename... Args> void set(T &&v, Args &&... args) {
    value<T>() = std::forward<T>(v);
    set(std::forward<Args>(args)...);
  }
};

} // namespace utility
} // namespace givr
//------------------------------------------------------------------------------
// END utility.h
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
// Start gl.h
//------------------------------------------------------------------------------
// TODO: figure out how to do all of this better.
#include <glad/glad.h>

// Shamelessly copied from GLFW
/* If we are we on Windows, we want a single define for it.
 *  */
#if !defined(_WIN32) &&                                                        \
    (defined(__WIN32__) || defined(WIN32) || defined(__MINGW32__))
#define _WIN32
#endif /* _WIN32 */

/* It is customary to use APIENTRY for OpenGL function pointer declarations on
 *  * all platforms.  Additionally, the Windows OpenGL header needs APIENTRY.
 *   */
#ifndef APIENTRY
#ifdef _WIN32
#define APIENTRY __stdcall
#else
#define APIENTRY
#endif
#define GIVR_APIENTRY_DEFINED
#endif /* APIENTRY */

/* Some Windows OpenGL headers need this.
 *  */
#if !defined(WINGDIAPI) && defined(_WIN32)
#define WINGDIAPI __declspec(dllimport)
#define GIVR_WINGDIAPI_DEFINED
#endif /* WINGDIAPI */

/* Some Windows GLU headers need this.
 *  */
#if !defined(CALLBACK) && defined(_WIN32)
#define CALLBACK __stdcall
#define GIVR_CALLBACK_DEFINED
#endif /* CALLBACK */

/* Include because most Windows GLU headers need wchar_t and
 *  * the macOS OpenGL header blocks the definition of ptrdiff_t by glext.h.
 *   * Include it unconditionally to avoid surprising side-effects.
 *    */
#include <stddef.h>

/* Include because it is needed by Vulkan and related functions.
 *  * Include it unconditionally to avoid surprising side-effects.
 *   */
#include <stdint.h>

/* Include the chosen OpenGL or OpenGL ES headers.
 *  */
#if defined(GIVR_INCLUDE_ES1)

#include <GLES/gl.h>
#if defined(GIVR_INCLUDE_GLEXT)
#include <GLES/glext.h>
#endif

#elif defined(GIVR_INCLUDE_ES2)

#include <GLES2/gl2.h>
#if defined(GIVR_INCLUDE_GLEXT)
#include <GLES2/gl2ext.h>
#endif

#elif defined(GIVR_INCLUDE_ES3)

#include <GLES3/gl3.h>
#if defined(GIVR_INCLUDE_GLEXT)
#include <GLES2/gl2ext.h>
#endif

#elif defined(GIVR_INCLUDE_ES31)

#include <GLES3/gl31.h>
#if defined(GIVR_INCLUDE_GLEXT)
#include <GLES2/gl2ext.h>
#endif

#elif defined(GIVR_INCLUDE_ES32)

#include <GLES3/gl32.h>
#if defined(GIVR_INCLUDE_GLEXT)
#include <GLES2/gl2ext.h>
#endif

#elif defined(GIVR_INCLUDE_GLCOREARB)

#if defined(__APPLE__)

#include <OpenGL/gl3.h>
#if defined(GIVR_INCLUDE_GLEXT)
#include <OpenGL/gl3ext.h>
#endif /*GIVR_INCLUDE_GLEXT*/

#else /*__APPLE__*/

#include <GL/glcorearb.h>

#endif /*__APPLE__*/

#elif !defined(GIVR_INCLUDE_NONE)

#if defined(__APPLE__)

#if !defined(GIVR_INCLUDE_GLEXT)
#define GL_GLEXT_LEGACY
#endif
#include <OpenGL/gl.h>
#if defined(GIVR_INCLUDE_GLU)
#include <OpenGL/glu.h>
#endif

#else /*__APPLE__*/

#include <GL/gl.h>
#if defined(GIVR_INCLUDE_GLEXT)
#include <GL/glext.h>
#endif
#if defined(GIVR_INCLUDE_GLU)
#include <GL/glu.h>
#endif

#endif /*__APPLE__*/

#endif /* OpenGL and OpenGL ES headers */

#if defined(GIVR_INCLUDE_VULKAN)
#include <vulkan/vulkan.h>
#endif /* Vulkan header */

//------------------------------------------------------------------------------
// END gl.h
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
// Start cube.h
//------------------------------------------------------------------------------
#include <cstdint>
#include <vector>

namespace givr {
struct cube {};

std::vector<float> generate_vertices(cube const &s);
std::vector<std::uint32_t> generate_indices(cube const &s);

// TODO: implement the uv and normal functions.
}; // end namespace givr
//------------------------------------------------------------------------------
// END cube.h
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
// Start static_assert.h
//------------------------------------------------------------------------------
#include <type_traits>

namespace givr {

// Checking for normals
template <typename T, typename = int> struct hasNormals : std::false_type {};
template <typename T>
struct hasNormals<T, decltype((void)T::Data::normals, 0)> : std::true_type {};

// Checking for vertices
template <typename T, typename = int> struct hasVertices : std::false_type {};
template <typename T>
struct hasVertices<T, decltype((void)T::Data::vertices, 0)> : std::true_type {};

// Checking for indices
template <typename T, typename = int> struct hasIndices : std::false_type {};
template <typename T>
struct hasIndices<T, decltype((void)T::Data::indices, 0)> : std::true_type {};

// Checking for colours
template <typename T, typename = int> struct hasColours : std::false_type {};
template <typename T>
struct hasColours<T, decltype((void)T::Data::colours, 0)> : std::true_type {};

// Checking for uvs
template <typename T, typename = int> struct hasUvs : std::false_type {};
template <typename T>
struct hasUvs<T, decltype((void)T::Data::uvs, 0)> : std::true_type {};
}; // namespace givr
//------------------------------------------------------------------------------
// END static_assert.h
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
// Start tiny_obj_loader.h
//------------------------------------------------------------------------------
/*
The MIT License (MIT)

Copyright (c) 2012-2018 Syoyo Fujita and many contributors.

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in
all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
THE SOFTWARE.
*/

//
// version 1.2.2 : Parse multiple group names.
// version 1.2.1 : Added initial support for line('l') primitive(PR #178)
// version 1.2.0 : Hardened implementation(#175)
// version 1.1.1 : Support smoothing groups(#162)
// version 1.1.0 : Support parsing vertex color(#144)
// version 1.0.8 : Fix parsing `g` tag just after `usemtl`(#138)
// version 1.0.7 : Support multiple tex options(#126)
// version 1.0.6 : Add TINYOBJLOADER_USE_DOUBLE option(#124)
// version 1.0.5 : Ignore `Tr` when `d` exists in MTL(#43)
// version 1.0.4 : Support multiple filenames for 'mtllib'(#112)
// version 1.0.3 : Support parsing texture options(#85)
// version 1.0.2 : Improve parsing speed by about a factor of 2 for large
// files(#105)
// version 1.0.1 : Fixes a shape is lost if obj ends with a 'usemtl'(#104)
// version 1.0.0 : Change data structure. Change license from BSD to MIT.
//

//
// Use this in *one* .cc
//   #define TINYOBJLOADER_IMPLEMENTATION
//   #include "tiny_obj_loader.h"
//

#ifndef TINY_OBJ_LOADER_H_
#define TINY_OBJ_LOADER_H_

#include <map>
#include <string>
#include <vector>

namespace tinyobj {

#ifdef __clang__
#pragma clang diagnostic push
#if __has_warning("-Wzero-as-null-pointer-constant")
#pragma clang diagnostic ignored "-Wzero-as-null-pointer-constant"
#endif

#pragma clang diagnostic ignored "-Wpadded"

#endif

// https://en.wikipedia.org/wiki/Wavefront_.obj_file says ...
//
//  -blendu on | off                       # set horizontal texture blending
//  (default on)
//  -blendv on | off                       # set vertical texture blending
//  (default on)
//  -boost real_value                      # boost mip-map sharpness
//  -mm base_value gain_value              # modify texture map values (default
//  0 1)
//                                         #     base_value = brightness,
//                                         gain_value = contrast
//  -o u [v [w]]                           # Origin offset             (default
//  0 0 0)
//  -s u [v [w]]                           # Scale                     (default
//  1 1 1)
//  -t u [v [w]]                           # Turbulence                (default
//  0 0 0)
//  -texres resolution                     # texture resolution to create
//  -clamp on | off                        # only render texels in the clamped
//  0-1 range (default off)
//                                         #   When unclamped, textures are
//                                         repeated across a surface,
//                                         #   when clamped, only texels which
//                                         fall within the 0-1
//                                         #   range are rendered.
//  -bm mult_value                         # bump multiplier (for bump maps
//  only)
//
//  -imfchan r | g | b | m | l | z         # specifies which channel of the file
//  is used to
//                                         # create a scalar or bump texture.
//                                         r:red, g:green,
//                                         # b:blue, m:matte, l:luminance,
//                                         z:z-depth..
//                                         # (the default for bump is 'l' and
//                                         for decal is 'm')
//  bump -imfchan r bumpmap.tga            # says to use the red channel of
//  bumpmap.tga as the bumpmap
//
// For reflection maps...
//
//   -type sphere                           # specifies a sphere for a "refl"
//   reflection map
//   -type cube_top    | cube_bottom |      # when using a cube map, the texture
//   file for each
//         cube_front  | cube_back   |      # side of the cube is specified
//         separately
//         cube_left   | cube_right

#ifdef TINYOBJLOADER_USE_DOUBLE
//#pragma message "using double"
typedef double real_t;
#else
//#pragma message "using float"
typedef float real_t;
#endif

typedef enum {
  TEXTURE_TYPE_NONE, // default
  TEXTURE_TYPE_SPHERE,
  TEXTURE_TYPE_CUBE_TOP,
  TEXTURE_TYPE_CUBE_BOTTOM,
  TEXTURE_TYPE_CUBE_FRONT,
  TEXTURE_TYPE_CUBE_BACK,
  TEXTURE_TYPE_CUBE_LEFT,
  TEXTURE_TYPE_CUBE_RIGHT
} texture_type_t;

typedef struct {
  texture_type_t type;     // -type (default TEXTURE_TYPE_NONE)
  real_t sharpness;        // -boost (default 1.0?)
  real_t brightness;       // base_value in -mm option (default 0)
  real_t contrast;         // gain_value in -mm option (default 1)
  real_t origin_offset[3]; // -o u [v [w]] (default 0 0 0)
  real_t scale[3];         // -s u [v [w]] (default 1 1 1)
  real_t turbulence[3];    // -t u [v [w]] (default 0 0 0)
  // int   texture_resolution; // -texres resolution (default = ?) TODO
  bool clamp;   // -clamp (default false)
  char imfchan; // -imfchan (the default for bump is 'l' and for decal is 'm')
  bool blendu;  // -blendu (default on)
  bool blendv;  // -blendv (default on)
  real_t bump_multiplier; // -bm (for bump maps only, default 1.0)
} texture_option_t;

typedef struct {
  std::string name;

  real_t ambient[3];
  real_t diffuse[3];
  real_t specular[3];
  real_t transmittance[3];
  real_t emission[3];
  real_t shininess;
  real_t ior;      // index of refraction
  real_t dissolve; // 1 == opaque; 0 == fully transparent
  // illumination model (see http://www.fileformat.info/format/material/)
  int illum;

  int dummy; // Suppress padding warning.

  std::string ambient_texname;            // map_Ka
  std::string diffuse_texname;            // map_Kd
  std::string specular_texname;           // map_Ks
  std::string specular_highlight_texname; // map_Ns
  std::string bump_texname;               // map_bump, map_Bump, bump
  std::string displacement_texname;       // disp
  std::string alpha_texname;              // map_d
  std::string reflection_texname;         // refl

  texture_option_t ambient_texopt;
  texture_option_t diffuse_texopt;
  texture_option_t specular_texopt;
  texture_option_t specular_highlight_texopt;
  texture_option_t bump_texopt;
  texture_option_t displacement_texopt;
  texture_option_t alpha_texopt;
  texture_option_t reflection_texopt;

  // PBR extension
  // http://exocortex.com/blog/extending_wavefront_mtl_to_support_pbr
  real_t roughness;           // [0, 1] default 0
  real_t metallic;            // [0, 1] default 0
  real_t sheen;               // [0, 1] default 0
  real_t clearcoat_thickness; // [0, 1] default 0
  real_t clearcoat_roughness; // [0, 1] default 0
  real_t anisotropy;          // aniso. [0, 1] default 0
  real_t anisotropy_rotation; // anisor. [0, 1] default 0
  real_t pad0;
  std::string roughness_texname; // map_Pr
  std::string metallic_texname;  // map_Pm
  std::string sheen_texname;     // map_Ps
  std::string emissive_texname;  // map_Ke
  std::string normal_texname;    // norm. For normal mapping.

  texture_option_t roughness_texopt;
  texture_option_t metallic_texopt;
  texture_option_t sheen_texopt;
  texture_option_t emissive_texopt;
  texture_option_t normal_texopt;

  int pad2;

  std::map<std::string, std::string> unknown_parameter;
} material_t;

typedef struct {
  std::string name;

  std::vector<int> intValues;
  std::vector<real_t> floatValues;
  std::vector<std::string> stringValues;
} tag_t;

// Index struct to support different indices for vtx/normal/texcoord.
// -1 means not used.
typedef struct {
  int vertex_index;
  int normal_index;
  int texcoord_index;
} index_t;

typedef struct {
  std::vector<index_t> indices;
  std::vector<unsigned char> num_face_vertices;  // The number of vertices per
                                                 // face. 3 = polygon, 4 = quad,
                                                 // ... Up to 255.
  std::vector<int> material_ids;                 // per-face material ID
  std::vector<unsigned int> smoothing_group_ids; // per-face smoothing group
                                                 // ID(0 = off. positive value
                                                 // = group id)
  std::vector<tag_t> tags;                       // SubD tag
} mesh_t;

typedef struct {
  std::vector<int> indices; // pairs of indices for lines
} path_t;

typedef struct {
  std::string name;
  mesh_t mesh;
  path_t path;
} shape_t;

// Vertex attributes
typedef struct {
  std::vector<real_t> vertices;  // 'v'
  std::vector<real_t> normals;   // 'vn'
  std::vector<real_t> texcoords; // 'vt'
  std::vector<real_t> colors;    // extension: vertex colors
} attrib_t;

typedef struct callback_t_ {
  // W is optional and set to 1 if there is no `w` item in `v` line
  void (*vertex_cb)(void *user_data, real_t x, real_t y, real_t z, real_t w);
  void (*normal_cb)(void *user_data, real_t x, real_t y, real_t z);

  // y and z are optional and set to 0 if there is no `y` and/or `z` item(s) in
  // `vt` line.
  void (*texcoord_cb)(void *user_data, real_t x, real_t y, real_t z);

  // called per 'f' line. num_indices is the number of face indices(e.g. 3 for
  // triangle, 4 for quad)
  // 0 will be passed for undefined index in index_t members.
  void (*index_cb)(void *user_data, index_t *indices, int num_indices);
  // `name` material name, `material_id` = the array index of material_t[]. -1
  // if
  // a material not found in .mtl
  void (*usemtl_cb)(void *user_data, const char *name, int material_id);
  // `materials` = parsed material data.
  void (*mtllib_cb)(void *user_data, const material_t *materials,
                    int num_materials);
  // There may be multiple group names
  void (*group_cb)(void *user_data, const char **names, int num_names);
  void (*object_cb)(void *user_data, const char *name);

  callback_t_()
      : vertex_cb(NULL), normal_cb(NULL), texcoord_cb(NULL), index_cb(NULL),
        usemtl_cb(NULL), mtllib_cb(NULL), group_cb(NULL), object_cb(NULL) {}
} callback_t;

class MaterialReader {
public:
  MaterialReader() {}
  virtual ~MaterialReader();

  virtual bool operator()(const std::string &matId,
                          std::vector<material_t> *materials,
                          std::map<std::string, int> *matMap,
                          std::string *err) = 0;
};

class MaterialFileReader : public MaterialReader {
public:
  explicit MaterialFileReader(const std::string &mtl_basedir)
      : m_mtlBaseDir(mtl_basedir) {}
  virtual ~MaterialFileReader() {}
  virtual bool operator()(const std::string &matId,
                          std::vector<material_t> *materials,
                          std::map<std::string, int> *matMap, std::string *err);

private:
  std::string m_mtlBaseDir;
};

class MaterialStreamReader : public MaterialReader {
public:
  explicit MaterialStreamReader(std::istream &inStream)
      : m_inStream(inStream) {}
  virtual ~MaterialStreamReader() {}
  virtual bool operator()(const std::string &matId,
                          std::vector<material_t> *materials,
                          std::map<std::string, int> *matMap, std::string *err);

private:
  std::istream &m_inStream;
};

/// Loads .obj from a file.
/// 'attrib', 'shapes' and 'materials' will be filled with parsed shape data
/// 'shapes' will be filled with parsed shape data
/// Returns true when loading .obj become success.
/// Returns warning and error message into `err`
/// 'mtl_basedir' is optional, and used for base directory for .mtl file.
/// In default(`NULL'), .mtl file is searched from an application's working
/// directory.
/// 'triangulate' is optional, and used whether triangulate polygon face in .obj
/// or not.
/// Option 'default_vcols_fallback' specifies whether vertex colors should
/// always be defined, even if no colors are given (fallback to white).
bool LoadObj(attrib_t *attrib, std::vector<shape_t> *shapes,
             std::vector<material_t> *materials, std::string *err,
             const char *filename, const char *mtl_basedir = NULL,
             bool triangulate = true, bool default_vcols_fallback = true);

/// Loads .obj from a file with custom user callback.
/// .mtl is loaded as usual and parsed material_t data will be passed to
/// `callback.mtllib_cb`.
/// Returns true when loading .obj/.mtl become success.
/// Returns warning and error message into `err`
/// See `examples/callback_api/` for how to use this function.
bool LoadObjWithCallback(std::istream &inStream, const callback_t &callback,
                         void *user_data = NULL,
                         MaterialReader *readMatFn = NULL,
                         std::string *err = NULL);

/// Loads object from a std::istream, uses GetMtlIStreamFn to retrieve
/// std::istream for materials.
/// Returns true when loading .obj become success.
/// Returns warning and error message into `err`
bool LoadObj(attrib_t *attrib, std::vector<shape_t> *shapes,
             std::vector<material_t> *materials, std::string *err,
             std::istream *inStream, MaterialReader *readMatFn = NULL,
             bool triangulate = true, bool default_vcols_fallback = true);

/// Loads materials into std::map
void LoadMtl(std::map<std::string, int> *material_map,
             std::vector<material_t> *materials, std::istream *inStream,
             std::string *warning);

} // namespace tinyobj

#endif // TINY_OBJ_LOADER_H_

#ifdef TINYOBJLOADER_IMPLEMENTATION
#include <cassert>
#include <cctype>
#include <cmath>
#include <cstddef>
#include <cstdlib>
#include <cstring>
#include <limits>
#include <utility>

#include <fstream>
#include <sstream>

namespace tinyobj {

MaterialReader::~MaterialReader() {}

struct vertex_index_t {
  int v_idx, vt_idx, vn_idx;
  vertex_index_t() : v_idx(-1), vt_idx(-1), vn_idx(-1) {}
  explicit vertex_index_t(int idx) : v_idx(idx), vt_idx(idx), vn_idx(idx) {}
  vertex_index_t(int vidx, int vtidx, int vnidx)
      : v_idx(vidx), vt_idx(vtidx), vn_idx(vnidx) {}
};

// Internal data structure for face representation
// index + smoothing group.
struct face_t {
  unsigned int
      smoothing_group_id; // smoothing group id. 0 = smoothing groupd is off.
  int pad_;
  std::vector<vertex_index_t> vertex_indices; // face vertex indices.

  face_t() : smoothing_group_id(0) {}
};

struct line_t {
  int idx0;
  int idx1;
};

struct tag_sizes {
  tag_sizes() : num_ints(0), num_reals(0), num_strings(0) {}
  int num_ints;
  int num_reals;
  int num_strings;
};

struct obj_shape {
  std::vector<real_t> v;
  std::vector<real_t> vn;
  std::vector<real_t> vt;
};

// See
// http://stackoverflow.com/questions/6089231/getting-std-ifstream-to-handle-lf-cr-and-crlf
static std::istream &safeGetline(std::istream &is, std::string &t) {
  t.clear();

  // The characters in the stream are read one-by-one using a std::streambuf.
  // That is faster than reading them one-by-one using the std::istream.
  // Code that uses streambuf this way must be guarded by a sentry object.
  // The sentry object performs various tasks,
  // such as thread synchronization and updating the stream state.

  std::istream::sentry se(is, true);
  std::streambuf *sb = is.rdbuf();

  if (se) {
    for (;;) {
      int c = sb->sbumpc();
      switch (c) {
      case '\n':
        return is;
      case '\r':
        if (sb->sgetc() == '\n')
          sb->sbumpc();
        return is;
      case EOF:
        // Also handle the case when the last line has no line ending
        if (t.empty())
          is.setstate(std::ios::eofbit);
        return is;
      default:
        t += static_cast<char>(c);
      }
    }
  }

  return is;
}

#define IS_SPACE(x) (((x) == ' ') || ((x) == '\t'))
#define IS_DIGIT(x)                                                            \
  (static_cast<unsigned int>((x) - '0') < static_cast<unsigned int>(10))
#define IS_NEW_LINE(x) (((x) == '\r') || ((x) == '\n') || ((x) == '\0'))

// Make index zero-base, and also support relative index.
static inline bool fixIndex(int idx, int n, int *ret) {
  if (!ret) {
    return false;
  }

  if (idx > 0) {
    (*ret) = idx - 1;
    return true;
  }

  if (idx == 0) {
    // zero is not allowed according to the spec.
    return false;
  }

  if (idx < 0) {
    (*ret) = n + idx; // negative value = relative
    return true;
  }

  return false; // never reach here.
}

static inline std::string parseString(const char **token) {
  std::string s;
  (*token) += strspn((*token), " \t");
  size_t e = strcspn((*token), " \t\r");
  s = std::string((*token), &(*token)[e]);
  (*token) += e;
  return s;
}

static inline int parseInt(const char **token) {
  (*token) += strspn((*token), " \t");
  int i = atoi((*token));
  (*token) += strcspn((*token), " \t\r");
  return i;
}

// Tries to parse a floating point number located at s.
//
// s_end should be a location in the string where reading should absolutely
// stop. For example at the end of the string, to prevent buffer overflows.
//
// Parses the following EBNF grammar:
//   sign    = "+" | "-" ;
//   END     = ? anything not in digit ?
//   digit   = "0" | "1" | "2" | "3" | "4" | "5" | "6" | "7" | "8" | "9" ;
//   integer = [sign] , digit , {digit} ;
//   decimal = integer , ["." , integer] ;
//   float   = ( decimal , END ) | ( decimal , ("E" | "e") , integer , END ) ;
//
//  Valid strings are for example:
//   -0  +3.1417e+2  -0.0E-3  1.0324  -1.41   11e2
//
// If the parsing is a success, result is set to the parsed value and true
// is returned.
//
// The function is greedy and will parse until any of the following happens:
//  - a non-conforming character is encountered.
//  - s_end is reached.
//
// The following situations triggers a failure:
//  - s >= s_end.
//  - parse failure.
//
static bool tryParseDouble(const char *s, const char *s_end, double *result) {
  if (s >= s_end) {
    return false;
  }

  double mantissa = 0.0;
  // This exponent is base 2 rather than 10.
  // However the exponent we parse is supposed to be one of ten,
  // thus we must take care to convert the exponent/and or the
  // mantissa to a * 2^E, where a is the mantissa and E is the
  // exponent.
  // To get the final double we will use ldexp, it requires the
  // exponent to be in base 2.
  int exponent = 0;

  // NOTE: THESE MUST BE DECLARED HERE SINCE WE ARE NOT ALLOWED
  // TO JUMP OVER DEFINITIONS.
  char sign = '+';
  char exp_sign = '+';
  char const *curr = s;

  // How many characters were read in a loop.
  int read = 0;
  // Tells whether a loop terminated due to reaching s_end.
  bool end_not_reached = false;

  /*
          BEGIN PARSING.
  */

  // Find out what sign we've got.
  if (*curr == '+' || *curr == '-') {
    sign = *curr;
    curr++;
  } else if (IS_DIGIT(*curr)) { /* Pass through. */
  } else {
    goto fail;
  }

  // Read the integer part.
  end_not_reached = (curr != s_end);
  while (end_not_reached && IS_DIGIT(*curr)) {
    mantissa *= 10;
    mantissa += static_cast<int>(*curr - 0x30);
    curr++;
    read++;
    end_not_reached = (curr != s_end);
  }

  // We must make sure we actually got something.
  if (read == 0)
    goto fail;
  // We allow numbers of form "#", "###" etc.
  if (!end_not_reached)
    goto assemble;

  // Read the decimal part.
  if (*curr == '.') {
    curr++;
    read = 1;
    end_not_reached = (curr != s_end);
    while (end_not_reached && IS_DIGIT(*curr)) {
      static const double pow_lut[] = {
          1.0, 0.1, 0.01, 0.001, 0.0001, 0.00001, 0.000001, 0.0000001,
      };
      const int lut_entries = sizeof pow_lut / sizeof pow_lut[0];

      // NOTE: Don't use powf here, it will absolutely murder precision.
      mantissa += static_cast<int>(*curr - 0x30) *
                  (read < lut_entries ? pow_lut[read] : std::pow(10.0, -read));
      read++;
      curr++;
      end_not_reached = (curr != s_end);
    }
  } else if (*curr == 'e' || *curr == 'E') {
  } else {
    goto assemble;
  }

  if (!end_not_reached)
    goto assemble;

  // Read the exponent part.
  if (*curr == 'e' || *curr == 'E') {
    curr++;
    // Figure out if a sign is present and if it is.
    end_not_reached = (curr != s_end);
    if (end_not_reached && (*curr == '+' || *curr == '-')) {
      exp_sign = *curr;
      curr++;
    } else if (IS_DIGIT(*curr)) { /* Pass through. */
    } else {
      // Empty E is not allowed.
      goto fail;
    }

    read = 0;
    end_not_reached = (curr != s_end);
    while (end_not_reached && IS_DIGIT(*curr)) {
      exponent *= 10;
      exponent += static_cast<int>(*curr - 0x30);
      curr++;
      read++;
      end_not_reached = (curr != s_end);
    }
    exponent *= (exp_sign == '+' ? 1 : -1);
    if (read == 0)
      goto fail;
  }

assemble:
  *result = (sign == '+' ? 1 : -1) *
            (exponent ? std::ldexp(mantissa * std::pow(5.0, exponent), exponent)
                      : mantissa);
  return true;
fail:
  return false;
}

static inline real_t parseReal(const char **token, double default_value = 0.0) {
  (*token) += strspn((*token), " \t");
  const char *end = (*token) + strcspn((*token), " \t\r");
  double val = default_value;
  tryParseDouble((*token), end, &val);
  real_t f = static_cast<real_t>(val);
  (*token) = end;
  return f;
}

static inline bool parseReal(const char **token, real_t *out) {
  (*token) += strspn((*token), " \t");
  const char *end = (*token) + strcspn((*token), " \t\r");
  double val;
  bool ret = tryParseDouble((*token), end, &val);
  if (ret) {
    real_t f = static_cast<real_t>(val);
    (*out) = f;
  }
  (*token) = end;
  return ret;
}

static inline void parseReal2(real_t *x, real_t *y, const char **token,
                              const double default_x = 0.0,
                              const double default_y = 0.0) {
  (*x) = parseReal(token, default_x);
  (*y) = parseReal(token, default_y);
}

static inline void parseReal3(real_t *x, real_t *y, real_t *z,
                              const char **token, const double default_x = 0.0,
                              const double default_y = 0.0,
                              const double default_z = 0.0) {
  (*x) = parseReal(token, default_x);
  (*y) = parseReal(token, default_y);
  (*z) = parseReal(token, default_z);
}

static inline void parseV(real_t *x, real_t *y, real_t *z, real_t *w,
                          const char **token, const double default_x = 0.0,
                          const double default_y = 0.0,
                          const double default_z = 0.0,
                          const double default_w = 1.0) {
  (*x) = parseReal(token, default_x);
  (*y) = parseReal(token, default_y);
  (*z) = parseReal(token, default_z);
  (*w) = parseReal(token, default_w);
}

// Extension: parse vertex with colors(6 items)
static inline bool parseVertexWithColor(real_t *x, real_t *y, real_t *z,
                                        real_t *r, real_t *g, real_t *b,
                                        const char **token,
                                        const double default_x = 0.0,
                                        const double default_y = 0.0,
                                        const double default_z = 0.0) {
  (*x) = parseReal(token, default_x);
  (*y) = parseReal(token, default_y);
  (*z) = parseReal(token, default_z);

  const bool found_color =
      parseReal(token, r) && parseReal(token, g) && parseReal(token, b);

  if (!found_color) {
    (*r) = (*g) = (*b) = 1.0;
  }

  return found_color;
}

static inline bool parseOnOff(const char **token, bool default_value = true) {
  (*token) += strspn((*token), " \t");
  const char *end = (*token) + strcspn((*token), " \t\r");

  bool ret = default_value;
  if ((0 == strncmp((*token), "on", 2))) {
    ret = true;
  } else if ((0 == strncmp((*token), "off", 3))) {
    ret = false;
  }

  (*token) = end;
  return ret;
}

static inline texture_type_t
parseTextureType(const char **token,
                 texture_type_t default_value = TEXTURE_TYPE_NONE) {
  (*token) += strspn((*token), " \t");
  const char *end = (*token) + strcspn((*token), " \t\r");
  texture_type_t ty = default_value;

  if ((0 == strncmp((*token), "cube_top", strlen("cube_top")))) {
    ty = TEXTURE_TYPE_CUBE_TOP;
  } else if ((0 == strncmp((*token), "cube_bottom", strlen("cube_bottom")))) {
    ty = TEXTURE_TYPE_CUBE_BOTTOM;
  } else if ((0 == strncmp((*token), "cube_left", strlen("cube_left")))) {
    ty = TEXTURE_TYPE_CUBE_LEFT;
  } else if ((0 == strncmp((*token), "cube_right", strlen("cube_right")))) {
    ty = TEXTURE_TYPE_CUBE_RIGHT;
  } else if ((0 == strncmp((*token), "cube_front", strlen("cube_front")))) {
    ty = TEXTURE_TYPE_CUBE_FRONT;
  } else if ((0 == strncmp((*token), "cube_back", strlen("cube_back")))) {
    ty = TEXTURE_TYPE_CUBE_BACK;
  } else if ((0 == strncmp((*token), "sphere", strlen("sphere")))) {
    ty = TEXTURE_TYPE_SPHERE;
  }

  (*token) = end;
  return ty;
}

static tag_sizes parseTagTriple(const char **token) {
  tag_sizes ts;

  (*token) += strspn((*token), " \t");
  ts.num_ints = atoi((*token));
  (*token) += strcspn((*token), "/ \t\r");
  if ((*token)[0] != '/') {
    return ts;
  }

  (*token)++; // Skip '/'

  (*token) += strspn((*token), " \t");
  ts.num_reals = atoi((*token));
  (*token) += strcspn((*token), "/ \t\r");
  if ((*token)[0] != '/') {
    return ts;
  }
  (*token)++; // Skip '/'

  ts.num_strings = parseInt(token);

  return ts;
}

// Parse triples with index offsets: i, i/j/k, i//k, i/j
static bool parseTriple(const char **token, int vsize, int vnsize, int vtsize,
                        vertex_index_t *ret) {
  if (!ret) {
    return false;
  }

  vertex_index_t vi(-1);

  if (!fixIndex(atoi((*token)), vsize, &(vi.v_idx))) {
    return false;
  }

  (*token) += strcspn((*token), "/ \t\r");
  if ((*token)[0] != '/') {
    (*ret) = vi;
    return true;
  }
  (*token)++;

  // i//k
  if ((*token)[0] == '/') {
    (*token)++;
    if (!fixIndex(atoi((*token)), vnsize, &(vi.vn_idx))) {
      return false;
    }
    (*token) += strcspn((*token), "/ \t\r");
    (*ret) = vi;
    return true;
  }

  // i/j/k or i/j
  if (!fixIndex(atoi((*token)), vtsize, &(vi.vt_idx))) {
    return false;
  }

  (*token) += strcspn((*token), "/ \t\r");
  if ((*token)[0] != '/') {
    (*ret) = vi;
    return true;
  }

  // i/j/k
  (*token)++; // skip '/'
  if (!fixIndex(atoi((*token)), vnsize, &(vi.vn_idx))) {
    return false;
  }
  (*token) += strcspn((*token), "/ \t\r");

  (*ret) = vi;

  return true;
}

// Parse raw triples: i, i/j/k, i//k, i/j
static vertex_index_t parseRawTriple(const char **token) {
  vertex_index_t vi(static_cast<int>(0)); // 0 is an invalid index in OBJ

  vi.v_idx = atoi((*token));
  (*token) += strcspn((*token), "/ \t\r");
  if ((*token)[0] != '/') {
    return vi;
  }
  (*token)++;

  // i//k
  if ((*token)[0] == '/') {
    (*token)++;
    vi.vn_idx = atoi((*token));
    (*token) += strcspn((*token), "/ \t\r");
    return vi;
  }

  // i/j/k or i/j
  vi.vt_idx = atoi((*token));
  (*token) += strcspn((*token), "/ \t\r");
  if ((*token)[0] != '/') {
    return vi;
  }

  // i/j/k
  (*token)++; // skip '/'
  vi.vn_idx = atoi((*token));
  (*token) += strcspn((*token), "/ \t\r");
  return vi;
}

static bool ParseTextureNameAndOption(std::string *texname,
                                      texture_option_t *texopt,
                                      const char *linebuf, const bool is_bump) {
  // @todo { write more robust lexer and parser. }
  bool found_texname = false;
  std::string texture_name;

  // Fill with default value for texopt.
  if (is_bump) {
    texopt->imfchan = 'l';
  } else {
    texopt->imfchan = 'm';
  }
  texopt->bump_multiplier = static_cast<real_t>(1.0);
  texopt->clamp = false;
  texopt->blendu = true;
  texopt->blendv = true;
  texopt->sharpness = static_cast<real_t>(1.0);
  texopt->brightness = static_cast<real_t>(0.0);
  texopt->contrast = static_cast<real_t>(1.0);
  texopt->origin_offset[0] = static_cast<real_t>(0.0);
  texopt->origin_offset[1] = static_cast<real_t>(0.0);
  texopt->origin_offset[2] = static_cast<real_t>(0.0);
  texopt->scale[0] = static_cast<real_t>(1.0);
  texopt->scale[1] = static_cast<real_t>(1.0);
  texopt->scale[2] = static_cast<real_t>(1.0);
  texopt->turbulence[0] = static_cast<real_t>(0.0);
  texopt->turbulence[1] = static_cast<real_t>(0.0);
  texopt->turbulence[2] = static_cast<real_t>(0.0);
  texopt->type = TEXTURE_TYPE_NONE;

  const char *token = linebuf; // Assume line ends with NULL

  while (!IS_NEW_LINE((*token))) {
    token += strspn(token, " \t"); // skip space
    if ((0 == strncmp(token, "-blendu", 7)) && IS_SPACE((token[7]))) {
      token += 8;
      texopt->blendu = parseOnOff(&token, /* default */ true);
    } else if ((0 == strncmp(token, "-blendv", 7)) && IS_SPACE((token[7]))) {
      token += 8;
      texopt->blendv = parseOnOff(&token, /* default */ true);
    } else if ((0 == strncmp(token, "-clamp", 6)) && IS_SPACE((token[6]))) {
      token += 7;
      texopt->clamp = parseOnOff(&token, /* default */ true);
    } else if ((0 == strncmp(token, "-boost", 6)) && IS_SPACE((token[6]))) {
      token += 7;
      texopt->sharpness = parseReal(&token, 1.0);
    } else if ((0 == strncmp(token, "-bm", 3)) && IS_SPACE((token[3]))) {
      token += 4;
      texopt->bump_multiplier = parseReal(&token, 1.0);
    } else if ((0 == strncmp(token, "-o", 2)) && IS_SPACE((token[2]))) {
      token += 3;
      parseReal3(&(texopt->origin_offset[0]), &(texopt->origin_offset[1]),
                 &(texopt->origin_offset[2]), &token);
    } else if ((0 == strncmp(token, "-s", 2)) && IS_SPACE((token[2]))) {
      token += 3;
      parseReal3(&(texopt->scale[0]), &(texopt->scale[1]), &(texopt->scale[2]),
                 &token, 1.0, 1.0, 1.0);
    } else if ((0 == strncmp(token, "-t", 2)) && IS_SPACE((token[2]))) {
      token += 3;
      parseReal3(&(texopt->turbulence[0]), &(texopt->turbulence[1]),
                 &(texopt->turbulence[2]), &token);
    } else if ((0 == strncmp(token, "-type", 5)) && IS_SPACE((token[5]))) {
      token += 5;
      texopt->type = parseTextureType((&token), TEXTURE_TYPE_NONE);
    } else if ((0 == strncmp(token, "-imfchan", 8)) && IS_SPACE((token[8]))) {
      token += 9;
      token += strspn(token, " \t");
      const char *end = token + strcspn(token, " \t\r");
      if ((end - token) == 1) { // Assume one char for -imfchan
        texopt->imfchan = (*token);
      }
      token = end;
    } else if ((0 == strncmp(token, "-mm", 3)) && IS_SPACE((token[3]))) {
      token += 4;
      parseReal2(&(texopt->brightness), &(texopt->contrast), &token, 0.0, 1.0);
    } else {
    // Assume texture filename
#if 0
      size_t len = strcspn(token, " \t\r");  // untile next space
      texture_name = std::string(token, token + len);
      token += len;

      token += strspn(token, " \t");  // skip space
#else
      // Read filename until line end to parse filename containing whitespace
      // TODO(syoyo): Support parsing texture option flag after the filename.
      texture_name = std::string(token);
      token += texture_name.length();
#endif

      found_texname = true;
    }
  }

  if (found_texname) {
    (*texname) = texture_name;
    return true;
  } else {
    return false;
  }
}

static void InitMaterial(material_t *material) {
  material->name = "";
  material->ambient_texname = "";
  material->diffuse_texname = "";
  material->specular_texname = "";
  material->specular_highlight_texname = "";
  material->bump_texname = "";
  material->displacement_texname = "";
  material->reflection_texname = "";
  material->alpha_texname = "";
  for (int i = 0; i < 3; i++) {
    material->ambient[i] = static_cast<real_t>(0.0);
    material->diffuse[i] = static_cast<real_t>(0.0);
    material->specular[i] = static_cast<real_t>(0.0);
    material->transmittance[i] = static_cast<real_t>(0.0);
    material->emission[i] = static_cast<real_t>(0.0);
  }
  material->illum = 0;
  material->dissolve = static_cast<real_t>(1.0);
  material->shininess = static_cast<real_t>(1.0);
  material->ior = static_cast<real_t>(1.0);

  material->roughness = static_cast<real_t>(0.0);
  material->metallic = static_cast<real_t>(0.0);
  material->sheen = static_cast<real_t>(0.0);
  material->clearcoat_thickness = static_cast<real_t>(0.0);
  material->clearcoat_roughness = static_cast<real_t>(0.0);
  material->anisotropy_rotation = static_cast<real_t>(0.0);
  material->anisotropy = static_cast<real_t>(0.0);
  material->roughness_texname = "";
  material->metallic_texname = "";
  material->sheen_texname = "";
  material->emissive_texname = "";
  material->normal_texname = "";

  material->unknown_parameter.clear();
}

// code from https://wrf.ecse.rpi.edu//Research/Short_Notes/pnpoly.html
template <typename T>
static int pnpoly(int nvert, T *vertx, T *verty, T testx, T testy) {
  int i, j, c = 0;
  for (i = 0, j = nvert - 1; i < nvert; j = i++) {
    if (((verty[i] > testy) != (verty[j] > testy)) &&
        (testx <
         (vertx[j] - vertx[i]) * (testy - verty[i]) / (verty[j] - verty[i]) +
             vertx[i]))
      c = !c;
  }
  return c;
}

// TODO(syoyo): refactor function.
static bool
exportGroupsToShape(shape_t *shape, const std::vector<face_t> &faceGroup,
                    std::vector<int> &lineGroup, const std::vector<tag_t> &tags,
                    const int material_id, const std::string &name,
                    bool triangulate, const std::vector<real_t> &v) {
  if (faceGroup.empty() && lineGroup.empty()) {
    return false;
  }

  if (!faceGroup.empty()) {
    // Flatten vertices and indices
    for (size_t i = 0; i < faceGroup.size(); i++) {
      const face_t &face = faceGroup[i];

      size_t npolys = face.vertex_indices.size();

      if (npolys < 3) {
        // Face must have 3+ vertices.
        continue;
      }

      vertex_index_t i0 = face.vertex_indices[0];
      vertex_index_t i1(-1);
      vertex_index_t i2 = face.vertex_indices[1];

      if (triangulate) {
        // find the two axes to work in
        size_t axes[2] = {1, 2};
        for (size_t k = 0; k < npolys; ++k) {
          i0 = face.vertex_indices[(k + 0) % npolys];
          i1 = face.vertex_indices[(k + 1) % npolys];
          i2 = face.vertex_indices[(k + 2) % npolys];
          size_t vi0 = size_t(i0.v_idx);
          size_t vi1 = size_t(i1.v_idx);
          size_t vi2 = size_t(i2.v_idx);

          if (((3 * vi0 + 2) >= v.size()) || ((3 * vi1 + 2) >= v.size()) ||
              ((3 * vi2 + 2) >= v.size())) {
            // Invalid triangle.
            // FIXME(syoyo): Is it ok to simply skip this invalid triangle?
            continue;
          }
          real_t v0x = v[vi0 * 3 + 0];
          real_t v0y = v[vi0 * 3 + 1];
          real_t v0z = v[vi0 * 3 + 2];
          real_t v1x = v[vi1 * 3 + 0];
          real_t v1y = v[vi1 * 3 + 1];
          real_t v1z = v[vi1 * 3 + 2];
          real_t v2x = v[vi2 * 3 + 0];
          real_t v2y = v[vi2 * 3 + 1];
          real_t v2z = v[vi2 * 3 + 2];
          real_t e0x = v1x - v0x;
          real_t e0y = v1y - v0y;
          real_t e0z = v1z - v0z;
          real_t e1x = v2x - v1x;
          real_t e1y = v2y - v1y;
          real_t e1z = v2z - v1z;
          real_t cx = std::fabs(e0y * e1z - e0z * e1y);
          real_t cy = std::fabs(e0z * e1x - e0x * e1z);
          real_t cz = std::fabs(e0x * e1y - e0y * e1x);
          const real_t epsilon = std::numeric_limits<real_t>::epsilon();
          if (cx > epsilon || cy > epsilon || cz > epsilon) {
            // found a corner
            if (cx > cy && cx > cz) {
            } else {
              axes[0] = 0;
              if (cz > cx && cz > cy)
                axes[1] = 1;
            }
            break;
          }
        }

        real_t area = 0;
        for (size_t k = 0; k < npolys; ++k) {
          i0 = face.vertex_indices[(k + 0) % npolys];
          i1 = face.vertex_indices[(k + 1) % npolys];
          size_t vi0 = size_t(i0.v_idx);
          size_t vi1 = size_t(i1.v_idx);
          if (((vi0 * 3 + axes[0]) >= v.size()) ||
              ((vi0 * 3 + axes[1]) >= v.size()) ||
              ((vi1 * 3 + axes[0]) >= v.size()) ||
              ((vi1 * 3 + axes[1]) >= v.size())) {
            // Invalid index.
            continue;
          }
          real_t v0x = v[vi0 * 3 + axes[0]];
          real_t v0y = v[vi0 * 3 + axes[1]];
          real_t v1x = v[vi1 * 3 + axes[0]];
          real_t v1y = v[vi1 * 3 + axes[1]];
          area += (v0x * v1y - v0y * v1x) * static_cast<real_t>(0.5);
        }

        int maxRounds = 10; // arbitrary max loop count to protect against
                            // unexpected errors

        face_t remainingFace = face; // copy
        size_t guess_vert = 0;
        vertex_index_t ind[3];
        real_t vx[3];
        real_t vy[3];
        while (remainingFace.vertex_indices.size() > 3 && maxRounds > 0) {
          npolys = remainingFace.vertex_indices.size();
          if (guess_vert >= npolys) {
            maxRounds -= 1;
            guess_vert -= npolys;
          }
          for (size_t k = 0; k < 3; k++) {
            ind[k] = remainingFace.vertex_indices[(guess_vert + k) % npolys];
            size_t vi = size_t(ind[k].v_idx);
            if (((vi * 3 + axes[0]) >= v.size()) ||
                ((vi * 3 + axes[1]) >= v.size())) {
              // ???
              vx[k] = static_cast<real_t>(0.0);
              vy[k] = static_cast<real_t>(0.0);
            } else {
              vx[k] = v[vi * 3 + axes[0]];
              vy[k] = v[vi * 3 + axes[1]];
            }
          }
          real_t e0x = vx[1] - vx[0];
          real_t e0y = vy[1] - vy[0];
          real_t e1x = vx[2] - vx[1];
          real_t e1y = vy[2] - vy[1];
          real_t cross = e0x * e1y - e0y * e1x;
          // if an internal angle
          if (cross * area < static_cast<real_t>(0.0)) {
            guess_vert += 1;
            continue;
          }

          // check all other verts in case they are inside this triangle
          bool overlap = false;
          for (size_t otherVert = 3; otherVert < npolys; ++otherVert) {
            size_t idx = (guess_vert + otherVert) % npolys;

            if (idx >= remainingFace.vertex_indices.size()) {
              // ???
              continue;
            }

            size_t ovi = size_t(remainingFace.vertex_indices[idx].v_idx);

            if (((ovi * 3 + axes[0]) >= v.size()) ||
                ((ovi * 3 + axes[1]) >= v.size())) {
              // ???
              continue;
            }
            real_t tx = v[ovi * 3 + axes[0]];
            real_t ty = v[ovi * 3 + axes[1]];
            if (pnpoly(3, vx, vy, tx, ty)) {
              overlap = true;
              break;
            }
          }

          if (overlap) {
            guess_vert += 1;
            continue;
          }

          // this triangle is an ear
          {
            index_t idx0, idx1, idx2;
            idx0.vertex_index = ind[0].v_idx;
            idx0.normal_index = ind[0].vn_idx;
            idx0.texcoord_index = ind[0].vt_idx;
            idx1.vertex_index = ind[1].v_idx;
            idx1.normal_index = ind[1].vn_idx;
            idx1.texcoord_index = ind[1].vt_idx;
            idx2.vertex_index = ind[2].v_idx;
            idx2.normal_index = ind[2].vn_idx;
            idx2.texcoord_index = ind[2].vt_idx;

            shape->mesh.indices.push_back(idx0);
            shape->mesh.indices.push_back(idx1);
            shape->mesh.indices.push_back(idx2);

            shape->mesh.num_face_vertices.push_back(3);
            shape->mesh.material_ids.push_back(material_id);
            shape->mesh.smoothing_group_ids.push_back(face.smoothing_group_id);
          }

          // remove v1 from the list
          size_t removed_vert_index = (guess_vert + 1) % npolys;
          while (removed_vert_index + 1 < npolys) {
            remainingFace.vertex_indices[removed_vert_index] =
                remainingFace.vertex_indices[removed_vert_index + 1];
            removed_vert_index += 1;
          }
          remainingFace.vertex_indices.pop_back();
        }

        if (remainingFace.vertex_indices.size() == 3) {
          i0 = remainingFace.vertex_indices[0];
          i1 = remainingFace.vertex_indices[1];
          i2 = remainingFace.vertex_indices[2];
          {
            index_t idx0, idx1, idx2;
            idx0.vertex_index = i0.v_idx;
            idx0.normal_index = i0.vn_idx;
            idx0.texcoord_index = i0.vt_idx;
            idx1.vertex_index = i1.v_idx;
            idx1.normal_index = i1.vn_idx;
            idx1.texcoord_index = i1.vt_idx;
            idx2.vertex_index = i2.v_idx;
            idx2.normal_index = i2.vn_idx;
            idx2.texcoord_index = i2.vt_idx;

            shape->mesh.indices.push_back(idx0);
            shape->mesh.indices.push_back(idx1);
            shape->mesh.indices.push_back(idx2);

            shape->mesh.num_face_vertices.push_back(3);
            shape->mesh.material_ids.push_back(material_id);
            shape->mesh.smoothing_group_ids.push_back(face.smoothing_group_id);
          }
        }
      } else {
        for (size_t k = 0; k < npolys; k++) {
          index_t idx;
          idx.vertex_index = face.vertex_indices[k].v_idx;
          idx.normal_index = face.vertex_indices[k].vn_idx;
          idx.texcoord_index = face.vertex_indices[k].vt_idx;
          shape->mesh.indices.push_back(idx);
        }

        shape->mesh.num_face_vertices.push_back(
            static_cast<unsigned char>(npolys));
        shape->mesh.material_ids.push_back(material_id); // per face
        shape->mesh.smoothing_group_ids.push_back(
            face.smoothing_group_id); // per face
      }
    }

    shape->name = name;
    shape->mesh.tags = tags;
  }

  if (!lineGroup.empty()) {
    shape->path.indices.swap(lineGroup);
  }

  return true;
}

// Split a string with specified delimiter character.
// http://stackoverflow.com/questions/236129/split-a-string-in-c
static void SplitString(const std::string &s, char delim,
                        std::vector<std::string> &elems) {
  std::stringstream ss;
  ss.str(s);
  std::string item;
  while (std::getline(ss, item, delim)) {
    elems.push_back(item);
  }
}

void LoadMtl(std::map<std::string, int> *material_map,
             std::vector<material_t> *materials, std::istream *inStream,
             std::string *warning) {
  // Create a default material anyway.
  material_t material;
  InitMaterial(&material);

  // Issue 43. `d` wins against `Tr` since `Tr` is not in the MTL specification.
  bool has_d = false;
  bool has_tr = false;

  std::stringstream ss;

  std::string linebuf;
  while (inStream->peek() != -1) {
    safeGetline(*inStream, linebuf);

    // Trim trailing whitespace.
    if (linebuf.size() > 0) {
      linebuf = linebuf.substr(0, linebuf.find_last_not_of(" \t") + 1);
    }

    // Trim newline '\r\n' or '\n'
    if (linebuf.size() > 0) {
      if (linebuf[linebuf.size() - 1] == '\n')
        linebuf.erase(linebuf.size() - 1);
    }
    if (linebuf.size() > 0) {
      if (linebuf[linebuf.size() - 1] == '\r')
        linebuf.erase(linebuf.size() - 1);
    }

    // Skip if empty line.
    if (linebuf.empty()) {
      continue;
    }

    // Skip leading space.
    const char *token = linebuf.c_str();
    token += strspn(token, " \t");

    assert(token);
    if (token[0] == '\0')
      continue; // empty line

    if (token[0] == '#')
      continue; // comment line

    // new mtl
    if ((0 == strncmp(token, "newmtl", 6)) && IS_SPACE((token[6]))) {
      // flush previous material.
      if (!material.name.empty()) {
        material_map->insert(std::pair<std::string, int>(
            material.name, static_cast<int>(materials->size())));
        materials->push_back(material);
      }

      // initial temporary material
      InitMaterial(&material);

      has_d = false;
      has_tr = false;

      // set new mtl name
      token += 7;
      {
        std::stringstream sstr;
        sstr << token;
        material.name = sstr.str();
      }
      continue;
    }

    // ambient
    if (token[0] == 'K' && token[1] == 'a' && IS_SPACE((token[2]))) {
      token += 2;
      real_t r, g, b;
      parseReal3(&r, &g, &b, &token);
      material.ambient[0] = r;
      material.ambient[1] = g;
      material.ambient[2] = b;
      continue;
    }

    // diffuse
    if (token[0] == 'K' && token[1] == 'd' && IS_SPACE((token[2]))) {
      token += 2;
      real_t r, g, b;
      parseReal3(&r, &g, &b, &token);
      material.diffuse[0] = r;
      material.diffuse[1] = g;
      material.diffuse[2] = b;
      continue;
    }

    // specular
    if (token[0] == 'K' && token[1] == 's' && IS_SPACE((token[2]))) {
      token += 2;
      real_t r, g, b;
      parseReal3(&r, &g, &b, &token);
      material.specular[0] = r;
      material.specular[1] = g;
      material.specular[2] = b;
      continue;
    }

    // transmittance
    if ((token[0] == 'K' && token[1] == 't' && IS_SPACE((token[2]))) ||
        (token[0] == 'T' && token[1] == 'f' && IS_SPACE((token[2])))) {
      token += 2;
      real_t r, g, b;
      parseReal3(&r, &g, &b, &token);
      material.transmittance[0] = r;
      material.transmittance[1] = g;
      material.transmittance[2] = b;
      continue;
    }

    // ior(index of refraction)
    if (token[0] == 'N' && token[1] == 'i' && IS_SPACE((token[2]))) {
      token += 2;
      material.ior = parseReal(&token);
      continue;
    }

    // emission
    if (token[0] == 'K' && token[1] == 'e' && IS_SPACE(token[2])) {
      token += 2;
      real_t r, g, b;
      parseReal3(&r, &g, &b, &token);
      material.emission[0] = r;
      material.emission[1] = g;
      material.emission[2] = b;
      continue;
    }

    // shininess
    if (token[0] == 'N' && token[1] == 's' && IS_SPACE(token[2])) {
      token += 2;
      material.shininess = parseReal(&token);
      continue;
    }

    // illum model
    if (0 == strncmp(token, "illum", 5) && IS_SPACE(token[5])) {
      token += 6;
      material.illum = parseInt(&token);
      continue;
    }

    // dissolve
    if ((token[0] == 'd' && IS_SPACE(token[1]))) {
      token += 1;
      material.dissolve = parseReal(&token);

      if (has_tr) {
        ss << "WARN: Both `d` and `Tr` parameters defined for \""
           << material.name << "\". Use the value of `d` for dissolve."
           << std::endl;
      }
      has_d = true;
      continue;
    }
    if (token[0] == 'T' && token[1] == 'r' && IS_SPACE(token[2])) {
      token += 2;
      if (has_d) {
        // `d` wins. Ignore `Tr` value.
        ss << "WARN: Both `d` and `Tr` parameters defined for \""
           << material.name << "\". Use the value of `d` for dissolve."
           << std::endl;
      } else {
        // We invert value of Tr(assume Tr is in range [0, 1])
        // NOTE: Interpretation of Tr is application(exporter) dependent. For
        // some application(e.g. 3ds max obj exporter), Tr = d(Issue 43)
        material.dissolve = static_cast<real_t>(1.0) - parseReal(&token);
      }
      has_tr = true;
      continue;
    }

    // PBR: roughness
    if (token[0] == 'P' && token[1] == 'r' && IS_SPACE(token[2])) {
      token += 2;
      material.roughness = parseReal(&token);
      continue;
    }

    // PBR: metallic
    if (token[0] == 'P' && token[1] == 'm' && IS_SPACE(token[2])) {
      token += 2;
      material.metallic = parseReal(&token);
      continue;
    }

    // PBR: sheen
    if (token[0] == 'P' && token[1] == 's' && IS_SPACE(token[2])) {
      token += 2;
      material.sheen = parseReal(&token);
      continue;
    }

    // PBR: clearcoat thickness
    if (token[0] == 'P' && token[1] == 'c' && IS_SPACE(token[2])) {
      token += 2;
      material.clearcoat_thickness = parseReal(&token);
      continue;
    }

    // PBR: clearcoat roughness
    if ((0 == strncmp(token, "Pcr", 3)) && IS_SPACE(token[3])) {
      token += 4;
      material.clearcoat_roughness = parseReal(&token);
      continue;
    }

    // PBR: anisotropy
    if ((0 == strncmp(token, "aniso", 5)) && IS_SPACE(token[5])) {
      token += 6;
      material.anisotropy = parseReal(&token);
      continue;
    }

    // PBR: anisotropy rotation
    if ((0 == strncmp(token, "anisor", 6)) && IS_SPACE(token[6])) {
      token += 7;
      material.anisotropy_rotation = parseReal(&token);
      continue;
    }

    // ambient texture
    if ((0 == strncmp(token, "map_Ka", 6)) && IS_SPACE(token[6])) {
      token += 7;
      ParseTextureNameAndOption(&(material.ambient_texname),
                                &(material.ambient_texopt), token,
                                /* is_bump */ false);
      continue;
    }

    // diffuse texture
    if ((0 == strncmp(token, "map_Kd", 6)) && IS_SPACE(token[6])) {
      token += 7;
      ParseTextureNameAndOption(&(material.diffuse_texname),
                                &(material.diffuse_texopt), token,
                                /* is_bump */ false);
      continue;
    }

    // specular texture
    if ((0 == strncmp(token, "map_Ks", 6)) && IS_SPACE(token[6])) {
      token += 7;
      ParseTextureNameAndOption(&(material.specular_texname),
                                &(material.specular_texopt), token,
                                /* is_bump */ false);
      continue;
    }

    // specular highlight texture
    if ((0 == strncmp(token, "map_Ns", 6)) && IS_SPACE(token[6])) {
      token += 7;
      ParseTextureNameAndOption(&(material.specular_highlight_texname),
                                &(material.specular_highlight_texopt), token,
                                /* is_bump */ false);
      continue;
    }

    // bump texture
    if ((0 == strncmp(token, "map_bump", 8)) && IS_SPACE(token[8])) {
      token += 9;
      ParseTextureNameAndOption(&(material.bump_texname),
                                &(material.bump_texopt), token,
                                /* is_bump */ true);
      continue;
    }

    // bump texture
    if ((0 == strncmp(token, "map_Bump", 8)) && IS_SPACE(token[8])) {
      token += 9;
      ParseTextureNameAndOption(&(material.bump_texname),
                                &(material.bump_texopt), token,
                                /* is_bump */ true);
      continue;
    }

    // bump texture
    if ((0 == strncmp(token, "bump", 4)) && IS_SPACE(token[4])) {
      token += 5;
      ParseTextureNameAndOption(&(material.bump_texname),
                                &(material.bump_texopt), token,
                                /* is_bump */ true);
      continue;
    }

    // alpha texture
    if ((0 == strncmp(token, "map_d", 5)) && IS_SPACE(token[5])) {
      token += 6;
      material.alpha_texname = token;
      ParseTextureNameAndOption(&(material.alpha_texname),
                                &(material.alpha_texopt), token,
                                /* is_bump */ false);
      continue;
    }

    // displacement texture
    if ((0 == strncmp(token, "disp", 4)) && IS_SPACE(token[4])) {
      token += 5;
      ParseTextureNameAndOption(&(material.displacement_texname),
                                &(material.displacement_texopt), token,
                                /* is_bump */ false);
      continue;
    }

    // reflection map
    if ((0 == strncmp(token, "refl", 4)) && IS_SPACE(token[4])) {
      token += 5;
      ParseTextureNameAndOption(&(material.reflection_texname),
                                &(material.reflection_texopt), token,
                                /* is_bump */ false);
      continue;
    }

    // PBR: roughness texture
    if ((0 == strncmp(token, "map_Pr", 6)) && IS_SPACE(token[6])) {
      token += 7;
      ParseTextureNameAndOption(&(material.roughness_texname),
                                &(material.roughness_texopt), token,
                                /* is_bump */ false);
      continue;
    }

    // PBR: metallic texture
    if ((0 == strncmp(token, "map_Pm", 6)) && IS_SPACE(token[6])) {
      token += 7;
      ParseTextureNameAndOption(&(material.metallic_texname),
                                &(material.metallic_texopt), token,
                                /* is_bump */ false);
      continue;
    }

    // PBR: sheen texture
    if ((0 == strncmp(token, "map_Ps", 6)) && IS_SPACE(token[6])) {
      token += 7;
      ParseTextureNameAndOption(&(material.sheen_texname),
                                &(material.sheen_texopt), token,
                                /* is_bump */ false);
      continue;
    }

    // PBR: emissive texture
    if ((0 == strncmp(token, "map_Ke", 6)) && IS_SPACE(token[6])) {
      token += 7;
      ParseTextureNameAndOption(&(material.emissive_texname),
                                &(material.emissive_texopt), token,
                                /* is_bump */ false);
      continue;
    }

    // PBR: normal map texture
    if ((0 == strncmp(token, "norm", 4)) && IS_SPACE(token[4])) {
      token += 5;
      ParseTextureNameAndOption(
          &(material.normal_texname), &(material.normal_texopt), token,
          /* is_bump */ false); // @fixme { is_bump will be true? }
      continue;
    }

    // unknown parameter
    const char *_space = strchr(token, ' ');
    if (!_space) {
      _space = strchr(token, '\t');
    }
    if (_space) {
      std::ptrdiff_t len = _space - token;
      std::string key(token, static_cast<size_t>(len));
      std::string value = _space + 1;
      material.unknown_parameter.insert(
          std::pair<std::string, std::string>(key, value));
    }
  }
  // flush last material.
  material_map->insert(std::pair<std::string, int>(
      material.name, static_cast<int>(materials->size())));
  materials->push_back(material);

  if (warning) {
    (*warning) = ss.str();
  }
}

bool MaterialFileReader::operator()(const std::string &matId,
                                    std::vector<material_t> *materials,
                                    std::map<std::string, int> *matMap,
                                    std::string *err) {
  std::string filepath;

  if (!m_mtlBaseDir.empty()) {
    filepath = std::string(m_mtlBaseDir) + matId;
  } else {
    filepath = matId;
  }

  std::ifstream matIStream(filepath.c_str());
  if (!matIStream) {
    std::stringstream ss;
    ss << "WARN: Material file [ " << filepath << " ] not found." << std::endl;
    if (err) {
      (*err) += ss.str();
    }
    return false;
  }

  std::string warning;
  LoadMtl(matMap, materials, &matIStream, &warning);

  if (!warning.empty()) {
    if (err) {
      (*err) += warning;
    }
  }

  return true;
}

bool MaterialStreamReader::operator()(const std::string &matId,
                                      std::vector<material_t> *materials,
                                      std::map<std::string, int> *matMap,
                                      std::string *err) {
  (void)matId;
  if (!m_inStream) {
    std::stringstream ss;
    ss << "WARN: Material stream in error state. " << std::endl;
    if (err) {
      (*err) += ss.str();
    }
    return false;
  }

  std::string warning;
  LoadMtl(matMap, materials, &m_inStream, &warning);

  if (!warning.empty()) {
    if (err) {
      (*err) += warning;
    }
  }

  return true;
}

bool LoadObj(attrib_t *attrib, std::vector<shape_t> *shapes,
             std::vector<material_t> *materials, std::string *err,
             const char *filename, const char *mtl_basedir, bool trianglulate,
             bool default_vcols_fallback) {
  attrib->vertices.clear();
  attrib->normals.clear();
  attrib->texcoords.clear();
  attrib->colors.clear();
  shapes->clear();

  std::stringstream errss;

  std::ifstream ifs(filename);
  if (!ifs) {
    errss << "Cannot open file [" << filename << "]" << std::endl;
    if (err) {
      (*err) = errss.str();
    }
    return false;
  }

  std::string baseDir = mtl_basedir ? mtl_basedir : "";
  if (!baseDir.empty()) {
#ifndef _WIN32
    const char dirsep = '/';
#else
    const char dirsep = '\\';
#endif
    if (baseDir[baseDir.length() - 1] != dirsep)
      baseDir += dirsep;
  }
  MaterialFileReader matFileReader(baseDir);

  return LoadObj(attrib, shapes, materials, err, &ifs, &matFileReader,
                 trianglulate, default_vcols_fallback);
}

bool LoadObj(attrib_t *attrib, std::vector<shape_t> *shapes,
             std::vector<material_t> *materials, std::string *err,
             std::istream *inStream, MaterialReader *readMatFn /*= NULL*/,
             bool triangulate, bool default_vcols_fallback) {
  std::stringstream errss;

  std::vector<real_t> v;
  std::vector<real_t> vn;
  std::vector<real_t> vt;
  std::vector<real_t> vc;
  std::vector<tag_t> tags;
  std::vector<face_t> faceGroup;
  std::vector<int> lineGroup;
  std::string name;

  // material
  std::map<std::string, int> material_map;
  int material = -1;

  // smoothing group id
  unsigned int current_smoothing_id = 0; // Initial value. 0 means no smoothing.

  int greatest_v_idx = -1;
  int greatest_vn_idx = -1;
  int greatest_vt_idx = -1;

  shape_t shape;

  bool found_all_colors = true;

  size_t line_num = 0;
  std::string linebuf;
  while (inStream->peek() != -1) {
    safeGetline(*inStream, linebuf);

    line_num++;

    // Trim newline '\r\n' or '\n'
    if (linebuf.size() > 0) {
      if (linebuf[linebuf.size() - 1] == '\n')
        linebuf.erase(linebuf.size() - 1);
    }
    if (linebuf.size() > 0) {
      if (linebuf[linebuf.size() - 1] == '\r')
        linebuf.erase(linebuf.size() - 1);
    }

    // Skip if empty line.
    if (linebuf.empty()) {
      continue;
    }

    // Skip leading space.
    const char *token = linebuf.c_str();
    token += strspn(token, " \t");

    assert(token);
    if (token[0] == '\0')
      continue; // empty line

    if (token[0] == '#')
      continue; // comment line

    // vertex
    if (token[0] == 'v' && IS_SPACE((token[1]))) {
      token += 2;
      real_t x, y, z;
      real_t r, g, b;

      found_all_colors &= parseVertexWithColor(&x, &y, &z, &r, &g, &b, &token);

      v.push_back(x);
      v.push_back(y);
      v.push_back(z);

      if (found_all_colors || default_vcols_fallback) {
        vc.push_back(r);
        vc.push_back(g);
        vc.push_back(b);
      }

      continue;
    }

    // normal
    if (token[0] == 'v' && token[1] == 'n' && IS_SPACE((token[2]))) {
      token += 3;
      real_t x, y, z;
      parseReal3(&x, &y, &z, &token);
      vn.push_back(x);
      vn.push_back(y);
      vn.push_back(z);
      continue;
    }

    // texcoord
    if (token[0] == 'v' && token[1] == 't' && IS_SPACE((token[2]))) {
      token += 3;
      real_t x, y;
      parseReal2(&x, &y, &token);
      vt.push_back(x);
      vt.push_back(y);
      continue;
    }

    // line
    if (token[0] == 'l' && IS_SPACE((token[1]))) {
      token += 2;

      line_t line_cache;
      bool end_line_bit = 0;
      while (!IS_NEW_LINE(token[0])) {
        // get index from string
        int idx = 0;
        fixIndex(parseInt(&token), 0, &idx);

        size_t n = strspn(token, " \t\r");
        token += n;

        if (!end_line_bit) {
          line_cache.idx0 = idx;
        } else {
          line_cache.idx1 = idx;
          lineGroup.push_back(line_cache.idx0);
          lineGroup.push_back(line_cache.idx1);
          line_cache = line_t();
        }
        end_line_bit = !end_line_bit;
      }

      continue;
    }
    // face
    if (token[0] == 'f' && IS_SPACE((token[1]))) {
      token += 2;
      token += strspn(token, " \t");

      face_t face;

      face.smoothing_group_id = current_smoothing_id;
      face.vertex_indices.reserve(3);

      while (!IS_NEW_LINE(token[0])) {
        vertex_index_t vi;
        if (!parseTriple(&token, static_cast<int>(v.size() / 3),
                         static_cast<int>(vn.size() / 3),
                         static_cast<int>(vt.size() / 2), &vi)) {
          if (err) {
            (*err) = "Failed parse `f' line(e.g. zero value for face index).\n";
          }
          return false;
        }

        greatest_v_idx = greatest_v_idx > vi.v_idx ? greatest_v_idx : vi.v_idx;
        greatest_vn_idx =
            greatest_vn_idx > vi.vn_idx ? greatest_vn_idx : vi.vn_idx;
        greatest_vt_idx =
            greatest_vt_idx > vi.vt_idx ? greatest_vt_idx : vi.vt_idx;

        face.vertex_indices.push_back(vi);
        size_t n = strspn(token, " \t\r");
        token += n;
      }

      // replace with emplace_back + std::move on C++11
      faceGroup.push_back(face);

      continue;
    }

    // use mtl
    if ((0 == strncmp(token, "usemtl", 6)) && IS_SPACE((token[6]))) {
      token += 7;
      std::stringstream ss;
      ss << token;
      std::string namebuf = ss.str();

      int newMaterialId = -1;
      if (material_map.find(namebuf) != material_map.end()) {
        newMaterialId = material_map[namebuf];
      } else {
        // { error!! material not found }
      }

      if (newMaterialId != material) {
        // Create per-face material. Thus we don't add `shape` to `shapes` at
        // this time.
        // just clear `faceGroup` after `exportGroupsToShape()` call.
        exportGroupsToShape(&shape, faceGroup, lineGroup, tags, material, name,
                            triangulate, v);
        faceGroup.clear();
        material = newMaterialId;
      }

      continue;
    }

    // load mtl
    if ((0 == strncmp(token, "mtllib", 6)) && IS_SPACE((token[6]))) {
      if (readMatFn) {
        token += 7;

        std::vector<std::string> filenames;
        SplitString(std::string(token), ' ', filenames);

        if (filenames.empty()) {
          if (err) {
            (*err) += "WARN: Looks like empty filename for mtllib. Use default "
                      "material. \n";
          }
        } else {
          bool found = false;
          for (size_t s = 0; s < filenames.size(); s++) {
            std::string err_mtl;
            bool ok = (*readMatFn)(filenames[s].c_str(), materials,
                                   &material_map, &err_mtl);
            if (err && (!err_mtl.empty())) {
              (*err) += err_mtl; // This should be warn message.
            }

            if (ok) {
              found = true;
              break;
            }
          }

          if (!found) {
            if (err) {
              (*err) += "WARN: Failed to load material file(s). Use default "
                        "material.\n";
            }
          }
        }
      }

      continue;
    }

    // group name
    if (token[0] == 'g' && IS_SPACE((token[1]))) {
      // flush previous face group.
      bool ret = exportGroupsToShape(&shape, faceGroup, lineGroup, tags,
                                     material, name, triangulate, v);
      (void)ret; // return value not used.

      if (shape.mesh.indices.size() > 0) {
        shapes->push_back(shape);
      }

      shape = shape_t();

      // material = -1;
      faceGroup.clear();

      std::vector<std::string> names;

      while (!IS_NEW_LINE(token[0])) {
        std::string str = parseString(&token);
        names.push_back(str);
        token += strspn(token, " \t\r"); // skip tag
      }

      // names[0] must be 'g'

      if (names.size() < 2) {
        // 'g' with empty names
        if (err) {
          std::stringstream ss;
          ss << "WARN: Empty group name. line: " << line_num << "\n";
          (*err) += ss.str();
          name = "";
        }
      } else {

        std::stringstream ss;
        ss << names[1];

        // tinyobjloader does not support multiple groups for a primitive.
        // Currently we concatinate multiple group names with a space to get
        // single group name.

        for (size_t i = 2; i < names.size(); i++) {
          ss << " " << names[i];
        }

        name = ss.str();
      }

      continue;
    }

    // object name
    if (token[0] == 'o' && IS_SPACE((token[1]))) {
      // flush previous face group.
      bool ret = exportGroupsToShape(&shape, faceGroup, lineGroup, tags,
                                     material, name, triangulate, v);
      if (ret) {
        shapes->push_back(shape);
      }

      // material = -1;
      faceGroup.clear();
      shape = shape_t();

      // @todo { multiple object name? }
      token += 2;
      std::stringstream ss;
      ss << token;
      name = ss.str();

      continue;
    }

    if (token[0] == 't' && IS_SPACE(token[1])) {
      const int max_tag_nums = 8192; // FIXME(syoyo): Parameterize.
      tag_t tag;

      token += 2;

      tag.name = parseString(&token);

      tag_sizes ts = parseTagTriple(&token);

      if (ts.num_ints < 0) {
        ts.num_ints = 0;
      }
      if (ts.num_ints > max_tag_nums) {
        ts.num_ints = max_tag_nums;
      }

      if (ts.num_reals < 0) {
        ts.num_reals = 0;
      }
      if (ts.num_reals > max_tag_nums) {
        ts.num_reals = max_tag_nums;
      }

      if (ts.num_strings < 0) {
        ts.num_strings = 0;
      }
      if (ts.num_strings > max_tag_nums) {
        ts.num_strings = max_tag_nums;
      }

      tag.intValues.resize(static_cast<size_t>(ts.num_ints));

      for (size_t i = 0; i < static_cast<size_t>(ts.num_ints); ++i) {
        tag.intValues[i] = parseInt(&token);
      }

      tag.floatValues.resize(static_cast<size_t>(ts.num_reals));
      for (size_t i = 0; i < static_cast<size_t>(ts.num_reals); ++i) {
        tag.floatValues[i] = parseReal(&token);
      }

      tag.stringValues.resize(static_cast<size_t>(ts.num_strings));
      for (size_t i = 0; i < static_cast<size_t>(ts.num_strings); ++i) {
        tag.stringValues[i] = parseString(&token);
      }

      tags.push_back(tag);

      continue;
    }

    if (token[0] == 's' && IS_SPACE(token[1])) {
      // smoothing group id
      token += 2;

      // skip space.
      token += strspn(token, " \t"); // skip space

      if (token[0] == '\0') {
        continue;
      }

      if (token[0] == '\r' || token[1] == '\n') {
        continue;
      }

      if (strlen(token) >= 3) {
        if (token[0] == 'o' && token[1] == 'f' && token[2] == 'f') {
          current_smoothing_id = 0;
        }
      } else {
        // assume number
        int smGroupId = parseInt(&token);
        if (smGroupId < 0) {
          // parse error. force set to 0.
          // FIXME(syoyo): Report warning.
          current_smoothing_id = 0;
        } else {
          current_smoothing_id = static_cast<unsigned int>(smGroupId);
        }
      }

      continue;
    } // smoothing group id

    // Ignore unknown command.
  }

  // not all vertices have colors, no default colors desired? -> clear colors
  if (!found_all_colors && !default_vcols_fallback) {
    vc.clear();
  }

  if (greatest_v_idx >= static_cast<int>(v.size() / 3)) {
    if (err) {
      std::stringstream ss;
      ss << "WARN: Vertex indices out of bounds.\n" << std::endl;
      (*err) += ss.str();
    }
  }
  if (greatest_vn_idx >= static_cast<int>(vn.size() / 3)) {
    if (err) {
      std::stringstream ss;
      ss << "WARN: Vertex normal indices out of bounds.\n" << std::endl;
      (*err) += ss.str();
    }
  }
  if (greatest_vt_idx >= static_cast<int>(vt.size() / 2)) {
    if (err) {
      std::stringstream ss;
      ss << "WARN: Vertex texcoord indices out of bounds.\n" << std::endl;
      (*err) += ss.str();
    }
  }

  bool ret = exportGroupsToShape(&shape, faceGroup, lineGroup, tags, material,
                                 name, triangulate, v);
  // exportGroupsToShape return false when `usemtl` is called in the last
  // line.
  // we also add `shape` to `shapes` when `shape.mesh` has already some
  // faces(indices)
  if (ret || shape.mesh.indices.size()) {
    shapes->push_back(shape);
  }
  faceGroup.clear(); // for safety

  if (err) {
    (*err) += errss.str();
  }

  attrib->vertices.swap(v);
  attrib->normals.swap(vn);
  attrib->texcoords.swap(vt);
  attrib->colors.swap(vc);

  return true;
}

bool LoadObjWithCallback(std::istream &inStream, const callback_t &callback,
                         void *user_data /*= NULL*/,
                         MaterialReader *readMatFn /*= NULL*/,
                         std::string *err /*= NULL*/) {
  std::stringstream errss;

  // material
  std::map<std::string, int> material_map;
  int material_id = -1; // -1 = invalid

  std::vector<index_t> indices;
  std::vector<material_t> materials;
  std::vector<std::string> names;
  names.reserve(2);
  std::vector<const char *> names_out;

  std::string linebuf;
  while (inStream.peek() != -1) {
    safeGetline(inStream, linebuf);

    // Trim newline '\r\n' or '\n'
    if (linebuf.size() > 0) {
      if (linebuf[linebuf.size() - 1] == '\n')
        linebuf.erase(linebuf.size() - 1);
    }
    if (linebuf.size() > 0) {
      if (linebuf[linebuf.size() - 1] == '\r')
        linebuf.erase(linebuf.size() - 1);
    }

    // Skip if empty line.
    if (linebuf.empty()) {
      continue;
    }

    // Skip leading space.
    const char *token = linebuf.c_str();
    token += strspn(token, " \t");

    assert(token);
    if (token[0] == '\0')
      continue; // empty line

    if (token[0] == '#')
      continue; // comment line

    // vertex
    if (token[0] == 'v' && IS_SPACE((token[1]))) {
      token += 2;
      // TODO(syoyo): Support parsing vertex color extension.
      real_t x, y, z, w; // w is optional. default = 1.0
      parseV(&x, &y, &z, &w, &token);
      if (callback.vertex_cb) {
        callback.vertex_cb(user_data, x, y, z, w);
      }
      continue;
    }

    // normal
    if (token[0] == 'v' && token[1] == 'n' && IS_SPACE((token[2]))) {
      token += 3;
      real_t x, y, z;
      parseReal3(&x, &y, &z, &token);
      if (callback.normal_cb) {
        callback.normal_cb(user_data, x, y, z);
      }
      continue;
    }

    // texcoord
    if (token[0] == 'v' && token[1] == 't' && IS_SPACE((token[2]))) {
      token += 3;
      real_t x, y, z; // y and z are optional. default = 0.0
      parseReal3(&x, &y, &z, &token);
      if (callback.texcoord_cb) {
        callback.texcoord_cb(user_data, x, y, z);
      }
      continue;
    }

    // face
    if (token[0] == 'f' && IS_SPACE((token[1]))) {
      token += 2;
      token += strspn(token, " \t");

      indices.clear();
      while (!IS_NEW_LINE(token[0])) {
        vertex_index_t vi = parseRawTriple(&token);

        index_t idx;
        idx.vertex_index = vi.v_idx;
        idx.normal_index = vi.vn_idx;
        idx.texcoord_index = vi.vt_idx;

        indices.push_back(idx);
        size_t n = strspn(token, " \t\r");
        token += n;
      }

      if (callback.index_cb && indices.size() > 0) {
        callback.index_cb(user_data, &indices.at(0),
                          static_cast<int>(indices.size()));
      }

      continue;
    }

    // use mtl
    if ((0 == strncmp(token, "usemtl", 6)) && IS_SPACE((token[6]))) {
      token += 7;
      std::stringstream ss;
      ss << token;
      std::string namebuf = ss.str();

      int newMaterialId = -1;
      if (material_map.find(namebuf) != material_map.end()) {
        newMaterialId = material_map[namebuf];
      } else {
        // { error!! material not found }
      }

      if (newMaterialId != material_id) {
        material_id = newMaterialId;
      }

      if (callback.usemtl_cb) {
        callback.usemtl_cb(user_data, namebuf.c_str(), material_id);
      }

      continue;
    }

    // load mtl
    if ((0 == strncmp(token, "mtllib", 6)) && IS_SPACE((token[6]))) {
      if (readMatFn) {
        token += 7;

        std::vector<std::string> filenames;
        SplitString(std::string(token), ' ', filenames);

        if (filenames.empty()) {
          if (err) {
            (*err) += "WARN: Looks like empty filename for mtllib. Use default "
                      "material. \n";
          }
        } else {
          bool found = false;
          for (size_t s = 0; s < filenames.size(); s++) {
            std::string err_mtl;
            bool ok = (*readMatFn)(filenames[s].c_str(), &materials,
                                   &material_map, &err_mtl);
            if (err && (!err_mtl.empty())) {
              (*err) += err_mtl; // This should be warn message.
            }

            if (ok) {
              found = true;
              break;
            }
          }

          if (!found) {
            if (err) {
              (*err) += "WARN: Failed to load material file(s). Use default "
                        "material.\n";
            }
          } else {
            if (callback.mtllib_cb) {
              callback.mtllib_cb(user_data, &materials.at(0),
                                 static_cast<int>(materials.size()));
            }
          }
        }
      }

      continue;
    }

    // group name
    if (token[0] == 'g' && IS_SPACE((token[1]))) {
      names.clear();

      while (!IS_NEW_LINE(token[0])) {
        std::string str = parseString(&token);
        names.push_back(str);
        token += strspn(token, " \t\r"); // skip tag
      }

      assert(names.size() > 0);

      if (callback.group_cb) {
        if (names.size() > 1) {
          // create const char* array.
          names_out.resize(names.size() - 1);
          for (size_t j = 0; j < names_out.size(); j++) {
            names_out[j] = names[j + 1].c_str();
          }
          callback.group_cb(user_data, &names_out.at(0),
                            static_cast<int>(names_out.size()));

        } else {
          callback.group_cb(user_data, NULL, 0);
        }
      }

      continue;
    }

    // object name
    if (token[0] == 'o' && IS_SPACE((token[1]))) {
      // @todo { multiple object name? }
      token += 2;

      std::stringstream ss;
      ss << token;
      std::string object_name = ss.str();

      if (callback.object_cb) {
        callback.object_cb(user_data, object_name.c_str());
      }

      continue;
    }

#if 0 // @todo
    if (token[0] == 't' && IS_SPACE(token[1])) {
      tag_t tag;

      token += 2;
      std::stringstream ss;
      ss << token;
      tag.name = ss.str();

      token += tag.name.size() + 1;

      tag_sizes ts = parseTagTriple(&token);

      tag.intValues.resize(static_cast<size_t>(ts.num_ints));

      for (size_t i = 0; i < static_cast<size_t>(ts.num_ints); ++i) {
        tag.intValues[i] = atoi(token);
        token += strcspn(token, "/ \t\r") + 1;
      }

      tag.floatValues.resize(static_cast<size_t>(ts.num_reals));
      for (size_t i = 0; i < static_cast<size_t>(ts.num_reals); ++i) {
        tag.floatValues[i] = parseReal(&token);
        token += strcspn(token, "/ \t\r") + 1;
      }

      tag.stringValues.resize(static_cast<size_t>(ts.num_strings));
      for (size_t i = 0; i < static_cast<size_t>(ts.num_strings); ++i) {
        std::stringstream ss;
        ss << token;
        tag.stringValues[i] = ss.str();
        token += tag.stringValues[i].size() + 1;
      }

      tags.push_back(tag);
    }
#endif

    // Ignore unknown command.
  }

  if (err) {
    (*err) += errss.str();
  }

  return true;
}

#ifdef __clang__
#pragma clang diagnostic pop
#endif
} // namespace tinyobj

#endif
//------------------------------------------------------------------------------
// END tiny_obj_loader.h
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
// Start stb_image.h
//------------------------------------------------------------------------------
/* stb_image - v2.16 - public domain image loader -
http://nothings.org/stb_image.h no warranty implied; use at your own risk

   Do this:
      #define STB_IMAGE_IMPLEMENTATION
   before you include this file in *one* C or C++ file to create the
implementation.

   // i.e. it should look like this:
   #include ...
   #include ...
   #include ...
   #define STB_IMAGE_IMPLEMENTATION

   You can #define STBI_ASSERT(x) before the #include to avoid using assert.h.
   And #define STBI_MALLOC, STBI_REALLOC, and STBI_FREE to avoid using
malloc,realloc,free


   QUICK NOTES:
      Primarily of interest to game developers and other people who can
          avoid problematic images and only need the trivial interface

      JPEG baseline & progressive (12 bpc/arithmetic not supported, same as
stock IJG lib) PNG 1/2/4/8/16-bit-per-channel

      TGA (not sure what subset, if a subset)
      BMP non-1bpp, non-RLE
      PSD (composited view only, no extra channels, 8/16 bit-per-channel)

      GIF (*comp always reports as 4-channel)
      HDR (radiance rgbE format)
      PIC (Softimage PIC)
      PNM (PPM and PGM binary only)

      Animated GIF still needs a proper API, but here's one way to do it:
          http://gist.github.com/urraka/685d9a6340b26b830d49

      - decode from memory or through FILE (define STBI_NO_STDIO to remove code)
      - decode from arbitrary I/O callbacks
      - SIMD acceleration on x86/x64 (SSE2) and ARM (NEON)

   Full documentation under "DOCUMENTATION" below.


LICENSE

  See end of file for license information.

RECENT REVISION HISTORY:

      2.16  (2017-07-23) all functions have 16-bit variants; optimizations;
bugfixes 2.15  (2017-03-18) fix png-1,2,4; all Imagenet JPGs; no runtime SSE
detection on GCC 2.14  (2017-03-03) remove deprecated STBI_JPEG_OLD; fixes for
Imagenet JPGs 2.13  (2016-12-04) experimental 16-bit API, only for PNG so far;
fixes 2.12  (2016-04-02) fix typo in 2.11 PSD fix that caused crashes 2.11
(2016-04-02) 16-bit PNGS; enable SSE2 in non-gcc x64 RGB-format JPEG; remove
white matting in PSD; allocate large structures on the stack; correct channel
count for PNG & BMP 2.10  (2016-01-22) avoid warning introduced in 2.09 2.09
(2016-01-16) 16-bit TGA; comments in PNM files; STBI_REALLOC_SIZED

   See end of file for full revision history.


 ============================    Contributors    =========================

 Image formats                          Extensions, features
    Sean Barrett (jpeg, png, bmp)          Jetro Lauha (stbi_info)
    Nicolas Schulz (hdr, psd)              Martin "SpartanJ" Golini (stbi_info)
    Jonathan Dummer (tga)                  James "moose2000" Brown (iPhone PNG)
    Jean-Marc Lienher (gif)                Ben "Disch" Wenger (io callbacks)
    Tom Seddon (pic)                       Omar Cornut (1/2/4-bit PNG)
    Thatcher Ulrich (psd)                  Nicolas Guillemot (vertical flip)
    Ken Miller (pgm, ppm)                  Richard Mitton (16-bit PSD)
    github:urraka (animated gif)           Junggon Kim (PNM comments)
                                           Daniel Gibson (16-bit TGA)
                                           socks-the-fox (16-bit PNG)
                                           Jeremy Sawicki (handle all ImageNet
JPGs) Optimizations & bugfixes Fabian "ryg" Giesen Arseny Kapoulkine John-Mark
Allen

 Bug & warning fixes
    Marc LeBlanc            David Woo          Guillaume George   Martins
Mozeiko Christpher Lloyd        Jerry Jansson      Joseph Thomson     Phil
Jordan Dave Moore              Roy Eltham         Hayaki Saito       Nathan Reed
    Won Chun                Luke Graham        Johan Duparc       Nick Verigakis
    the Horde3D community   Thomas Ruf         Ronny Chevalier    Baldur
Karlsson Janez Zemva             John Bartholomew   Michal Cichon
github:rlyeh Jonathan Blow           Ken Hamada         Tero Hanninen
github:romigrou Laurent Gomila          Cort Stratton      Sergio Gonzalez
github:svdijk Aruelien Pocheville     Thibault Reuille   Cass Everitt
github:snagar Ryamond Barbiero        Paul Du Bois       Engin Manap
github:Zelex Michaelangel007@github  Philipp Wiesemann  Dale Weiler
github:grim210 Oriol Ferrer Mesia      Josh Tobin         Matthew Gregan
github:sammyhw Blazej Dariusz Roszkowski                  Gregory Mullen
github:phprus Christian Floisand      Kevin Schmidt
github:poppolopoppo
*/

#ifndef STBI_INCLUDE_STB_IMAGE_H
#define STBI_INCLUDE_STB_IMAGE_H

// DOCUMENTATION
//
// Limitations:
//    - no 16-bit-per-channel PNG
//    - no 12-bit-per-channel JPEG
//    - no JPEGs with arithmetic coding
//    - no 1-bit BMP
//    - GIF always returns *comp=4
//
// Basic usage (see HDR discussion below for HDR usage):
//    int x,y,n;
//    unsigned char *data = stbi_load(filename, &x, &y, &n, 0);
//    // ... process data if not NULL ...
//    // ... x = width, y = height, n = # 8-bit components per pixel ...
//    // ... replace '0' with '1'..'4' to force that many components per pixel
//    // ... but 'n' will always be the number that it would have been if you
//    said 0 stbi_image_free(data)
//
// Standard parameters:
//    int *x                 -- outputs image width in pixels
//    int *y                 -- outputs image height in pixels
//    int *channels_in_file  -- outputs # of image components in image file
//    int desired_channels   -- if non-zero, # of image components requested in
//    result
//
// The return value from an image loader is an 'unsigned char *' which points
// to the pixel data, or NULL on an allocation failure or if the image is
// corrupt or invalid. The pixel data consists of *y scanlines of *x pixels,
// with each pixel consisting of N interleaved 8-bit components; the first
// pixel pointed to is top-left-most in the image. There is no padding between
// image scanlines or between pixels, regardless of format. The number of
// components N is 'desired_channels' if desired_channels is non-zero, or
// *channels_in_file otherwise. If desired_channels is non-zero,
// *channels_in_file has the number of components that _would_ have been
// output otherwise. E.g. if you set desired_channels to 4, you will always
// get RGBA output, but you can check *channels_in_file to see if it's trivially
// opaque because e.g. there were only 3 channels in the source image.
//
// An output image with N components has the following components interleaved
// in this order in each pixel:
//
//     N=#comp     components
//       1           grey
//       2           grey, alpha
//       3           red, green, blue
//       4           red, green, blue, alpha
//
// If image loading fails for any reason, the return value will be NULL,
// and *x, *y, *channels_in_file will be unchanged. The function
// stbi_failure_reason() can be queried for an extremely brief, end-user
// unfriendly explanation of why the load failed. Define STBI_NO_FAILURE_STRINGS
// to avoid compiling these strings at all, and STBI_FAILURE_USERMSG to get
// slightly more user-friendly ones.
//
// Paletted PNG, BMP, GIF, and PIC images are automatically depalettized.
//
// ===========================================================================
//
// Philosophy
//
// stb libraries are designed with the following priorities:
//
//    1. easy to use
//    2. easy to maintain
//    3. good performance
//
// Sometimes I let "good performance" creep up in priority over "easy to
// maintain", and for best performance I may provide less-easy-to-use APIs that
// give higher performance, in addition to the easy to use ones. Nevertheless,
// it's important to keep in mind that from the standpoint of you, a client of
// this library, all you care about is #1 and #3, and stb libraries DO NOT
// emphasize #3 above all.
//
// Some secondary priorities arise directly from the first two, some of which
// make more explicit reasons why performance can't be emphasized.
//
//    - Portable ("ease of use")
//    - Small source code footprint ("easy to maintain")
//    - No dependencies ("ease of use")
//
// ===========================================================================
//
// I/O callbacks
//
// I/O callbacks allow you to read from arbitrary sources, like packaged
// files or some other source. Data read from callbacks are processed
// through a small internal buffer (currently 128 bytes) to try to reduce
// overhead.
//
// The three functions you must define are "read" (reads some bytes of data),
// "skip" (skips some bytes of data), "eof" (reports if the stream is at the
// end).
//
// ===========================================================================
//
// SIMD support
//
// The JPEG decoder will try to automatically use SIMD kernels on x86 when
// supported by the compiler. For ARM Neon support, you must explicitly
// request it.
//
// (The old do-it-yourself SIMD API is no longer supported in the current
// code.)
//
// On x86, SSE2 will automatically be used when available based on a run-time
// test; if not, the generic C versions are used as a fall-back. On ARM targets,
// the typical path is to have separate builds for NEON and non-NEON devices
// (at least this is true for iOS and Android). Therefore, the NEON support is
// toggled by a build flag: define STBI_NEON to get NEON loops.
//
// If for some reason you do not want to use any of SIMD code, or if
// you have issues compiling it, you can disable it entirely by
// defining STBI_NO_SIMD.
//
// ===========================================================================
//
// HDR image support   (disable by defining STBI_NO_HDR)
//
// stb_image now supports loading HDR images in general, and currently
// the Radiance .HDR file format, although the support is provided
// generically. You can still load any file through the existing interface;
// if you attempt to load an HDR file, it will be automatically remapped to
// LDR, assuming gamma 2.2 and an arbitrary scale factor defaulting to 1;
// both of these constants can be reconfigured through this interface:
//
//     stbi_hdr_to_ldr_gamma(2.2f);
//     stbi_hdr_to_ldr_scale(1.0f);
//
// (note, do not use _inverse_ constants; stbi_image will invert them
// appropriately).
//
// Additionally, there is a new, parallel interface for loading files as
// (linear) floats to preserve the full dynamic range:
//
//    float *data = stbi_loadf(filename, &x, &y, &n, 0);
//
// If you load LDR images through this interface, those images will
// be promoted to floating point values, run through the inverse of
// constants corresponding to the above:
//
//     stbi_ldr_to_hdr_scale(1.0f);
//     stbi_ldr_to_hdr_gamma(2.2f);
//
// Finally, given a filename (or an open file or memory block--see header
// file for details) containing image data, you can query for the "most
// appropriate" interface to use (that is, whether the image is HDR or
// not), using:
//
//     stbi_is_hdr(char *filename);
//
// ===========================================================================
//
// iPhone PNG support:
//
// By default we convert iphone-formatted PNGs back to RGB, even though
// they are internally encoded differently. You can disable this conversion
// by by calling stbi_convert_iphone_png_to_rgb(0), in which case
// you will always just get the native iphone "format" through (which
// is BGR stored in RGB).
//
// Call stbi_set_unpremultiply_on_load(1) as well to force a divide per
// pixel to remove any premultiplied alpha *only* if the image file explicitly
// says there's premultiplied data (currently only happens in iPhone images,
// and only if iPhone convert-to-rgb processing is on).
//
// ===========================================================================
//
// ADDITIONAL CONFIGURATION
//
//  - You can suppress implementation of any of the decoders to reduce
//    your code footprint by #defining one or more of the following
//    symbols before creating the implementation.
//
//        STBI_NO_JPEG
//        STBI_NO_PNG
//        STBI_NO_BMP
//        STBI_NO_PSD
//        STBI_NO_TGA
//        STBI_NO_GIF
//        STBI_NO_HDR
//        STBI_NO_PIC
//        STBI_NO_PNM   (.ppm and .pgm)
//
//  - You can request *only* certain decoders and suppress all other ones
//    (this will be more forward-compatible, as addition of new decoders
//    doesn't require you to disable them explicitly):
//
//        STBI_ONLY_JPEG
//        STBI_ONLY_PNG
//        STBI_ONLY_BMP
//        STBI_ONLY_PSD
//        STBI_ONLY_TGA
//        STBI_ONLY_GIF
//        STBI_ONLY_HDR
//        STBI_ONLY_PIC
//        STBI_ONLY_PNM   (.ppm and .pgm)
//
//   - If you use STBI_NO_PNG (or _ONLY_ without PNG), and you still
//     want the zlib decoder to be available, #define STBI_SUPPORT_ZLIB
//

#ifndef STBI_NO_STDIO
#include <stdio.h>
#endif // STBI_NO_STDIO

#define STBI_VERSION 1

enum {
  STBI_default = 0, // only used for desired_channels

  STBI_grey = 1,
  STBI_grey_alpha = 2,
  STBI_rgb = 3,
  STBI_rgb_alpha = 4
};

typedef unsigned char stbi_uc;
typedef unsigned short stbi_us;

#ifdef __cplusplus
extern "C" {
#endif

#ifdef STB_IMAGE_STATIC
#define STBIDEF static
#else
#define STBIDEF extern
#endif

//////////////////////////////////////////////////////////////////////////////
//
// PRIMARY API - works on images of any type
//

//
// load image by filename, open file, or memory buffer
//

typedef struct {
  int (*read)(void *user, char *data, int size); // fill 'data' with 'size'
                                                 // bytes.  return number of
                                                 // bytes actually read
  void (*skip)(void *user, int n); // skip the next 'n' bytes, or 'unget' the
                                   // last -n bytes if negative
  int (*eof)(void *user); // returns nonzero if we are at end of file/data
} stbi_io_callbacks;

////////////////////////////////////
//
// 8-bits-per-channel interface
//

STBIDEF stbi_uc *stbi_load_from_memory(stbi_uc const *buffer, int len, int *x,
                                       int *y, int *channels_in_file,
                                       int desired_channels);
STBIDEF stbi_uc *stbi_load_from_callbacks(stbi_io_callbacks const *clbk,
                                          void *user, int *x, int *y,
                                          int *channels_in_file,
                                          int desired_channels);

#ifndef STBI_NO_STDIO
STBIDEF stbi_uc *stbi_load(char const *filename, int *x, int *y,
                           int *channels_in_file, int desired_channels);
STBIDEF stbi_uc *stbi_load_from_file(FILE *f, int *x, int *y,
                                     int *channels_in_file,
                                     int desired_channels);
// for stbi_load_from_file, file pointer is left pointing immediately after
// image
#endif

////////////////////////////////////
//
// 16-bits-per-channel interface
//

STBIDEF stbi_us *stbi_load_16_from_memory(stbi_uc const *buffer, int len,
                                          int *x, int *y, int *channels_in_file,
                                          int desired_channels);
STBIDEF stbi_us *stbi_load_16_from_callbacks(stbi_io_callbacks const *clbk,
                                             void *user, int *x, int *y,
                                             int *channels_in_file,
                                             int desired_channels);

#ifndef STBI_NO_STDIO
STBIDEF stbi_us *stbi_load_16(char const *filename, int *x, int *y,
                              int *channels_in_file, int desired_channels);
STBIDEF stbi_us *stbi_load_from_file_16(FILE *f, int *x, int *y,
                                        int *channels_in_file,
                                        int desired_channels);
#endif

////////////////////////////////////
//
// float-per-channel interface
//
#ifndef STBI_NO_LINEAR
STBIDEF float *stbi_loadf_from_memory(stbi_uc const *buffer, int len, int *x,
                                      int *y, int *channels_in_file,
                                      int desired_channels);
STBIDEF float *stbi_loadf_from_callbacks(stbi_io_callbacks const *clbk,
                                         void *user, int *x, int *y,
                                         int *channels_in_file,
                                         int desired_channels);

#ifndef STBI_NO_STDIO
STBIDEF float *stbi_loadf(char const *filename, int *x, int *y,
                          int *channels_in_file, int desired_channels);
STBIDEF float *stbi_loadf_from_file(FILE *f, int *x, int *y,
                                    int *channels_in_file,
                                    int desired_channels);
#endif
#endif

#ifndef STBI_NO_HDR
STBIDEF void stbi_hdr_to_ldr_gamma(float gamma);
STBIDEF void stbi_hdr_to_ldr_scale(float scale);
#endif // STBI_NO_HDR

#ifndef STBI_NO_LINEAR
STBIDEF void stbi_ldr_to_hdr_gamma(float gamma);
STBIDEF void stbi_ldr_to_hdr_scale(float scale);
#endif // STBI_NO_LINEAR

// stbi_is_hdr is always defined, but always returns false if STBI_NO_HDR
STBIDEF int stbi_is_hdr_from_callbacks(stbi_io_callbacks const *clbk,
                                       void *user);
STBIDEF int stbi_is_hdr_from_memory(stbi_uc const *buffer, int len);
#ifndef STBI_NO_STDIO
STBIDEF int stbi_is_hdr(char const *filename);
STBIDEF int stbi_is_hdr_from_file(FILE *f);
#endif // STBI_NO_STDIO

// get a VERY brief reason for failure
// NOT THREADSAFE
STBIDEF const char *stbi_failure_reason(void);

// free the loaded image -- this is just free()
STBIDEF void stbi_image_free(void *retval_from_stbi_load);

// get image dimensions & components without fully decoding
STBIDEF int stbi_info_from_memory(stbi_uc const *buffer, int len, int *x,
                                  int *y, int *comp);
STBIDEF int stbi_info_from_callbacks(stbi_io_callbacks const *clbk, void *user,
                                     int *x, int *y, int *comp);

#ifndef STBI_NO_STDIO
STBIDEF int stbi_info(char const *filename, int *x, int *y, int *comp);
STBIDEF int stbi_info_from_file(FILE *f, int *x, int *y, int *comp);

#endif

// for image formats that explicitly notate that they have premultiplied alpha,
// we just return the colors as stored in the file. set this flag to force
// unpremultiplication. results are undefined if the unpremultiply overflow.
STBIDEF void
stbi_set_unpremultiply_on_load(int flag_true_if_should_unpremultiply);

// indicate whether we should process iphone images back to canonical format,
// or just pass them through "as-is"
STBIDEF void stbi_convert_iphone_png_to_rgb(int flag_true_if_should_convert);

// flip the image vertically, so the first pixel in the output array is the
// bottom left
STBIDEF void stbi_set_flip_vertically_on_load(int flag_true_if_should_flip);

// ZLIB client - used by PNG, available for other purposes

STBIDEF char *stbi_zlib_decode_malloc_guesssize(const char *buffer, int len,
                                                int initial_size, int *outlen);
STBIDEF char *stbi_zlib_decode_malloc_guesssize_headerflag(const char *buffer,
                                                           int len,
                                                           int initial_size,
                                                           int *outlen,
                                                           int parse_header);
STBIDEF char *stbi_zlib_decode_malloc(const char *buffer, int len, int *outlen);
STBIDEF int stbi_zlib_decode_buffer(char *obuffer, int olen,
                                    const char *ibuffer, int ilen);

STBIDEF char *stbi_zlib_decode_noheader_malloc(const char *buffer, int len,
                                               int *outlen);
STBIDEF int stbi_zlib_decode_noheader_buffer(char *obuffer, int olen,
                                             const char *ibuffer, int ilen);

#ifdef __cplusplus
}
#endif

//
//
////   end header file   /////////////////////////////////////////////////////
#endif // STBI_INCLUDE_STB_IMAGE_H

#ifdef STB_IMAGE_IMPLEMENTATION

#if defined(STBI_ONLY_JPEG) || defined(STBI_ONLY_PNG) ||                       \
    defined(STBI_ONLY_BMP) || defined(STBI_ONLY_TGA) ||                        \
    defined(STBI_ONLY_GIF) || defined(STBI_ONLY_PSD) ||                        \
    defined(STBI_ONLY_HDR) || defined(STBI_ONLY_PIC) ||                        \
    defined(STBI_ONLY_PNM) || defined(STBI_ONLY_ZLIB)
#ifndef STBI_ONLY_JPEG
#define STBI_NO_JPEG
#endif
#ifndef STBI_ONLY_PNG
#define STBI_NO_PNG
#endif
#ifndef STBI_ONLY_BMP
#define STBI_NO_BMP
#endif
#ifndef STBI_ONLY_PSD
#define STBI_NO_PSD
#endif
#ifndef STBI_ONLY_TGA
#define STBI_NO_TGA
#endif
#ifndef STBI_ONLY_GIF
#define STBI_NO_GIF
#endif
#ifndef STBI_ONLY_HDR
#define STBI_NO_HDR
#endif
#ifndef STBI_ONLY_PIC
#define STBI_NO_PIC
#endif
#ifndef STBI_ONLY_PNM
#define STBI_NO_PNM
#endif
#endif

#if defined(STBI_NO_PNG) && !defined(STBI_SUPPORT_ZLIB) &&                     \
    !defined(STBI_NO_ZLIB)
#define STBI_NO_ZLIB
#endif

#include <limits.h>
#include <stdarg.h>
#include <stddef.h> // ptrdiff_t on osx
#include <stdlib.h>
#include <string.h>

#if !defined(STBI_NO_LINEAR) || !defined(STBI_NO_HDR)
#include <math.h> // ldexp
#endif

#ifndef STBI_NO_STDIO
#include <stdio.h>
#endif

#ifndef STBI_ASSERT
#include <assert.h>
#define STBI_ASSERT(x) assert(x)
#endif

#ifndef _MSC_VER
#ifdef __cplusplus
#define stbi_inline inline
#else
#define stbi_inline
#endif
#else
#define stbi_inline __forceinline
#endif

#ifdef _MSC_VER
typedef unsigned short stbi__uint16;
typedef signed short stbi__int16;
typedef unsigned int stbi__uint32;
typedef signed int stbi__int32;
#else
#include <stdint.h>
typedef uint16_t stbi__uint16;
typedef int16_t stbi__int16;
typedef uint32_t stbi__uint32;
typedef int32_t stbi__int32;
#endif

// should produce compiler error if size is wrong
typedef unsigned char validate_uint32[sizeof(stbi__uint32) == 4 ? 1 : -1];

#ifdef _MSC_VER
#define STBI_NOTUSED(v) (void)(v)
#else
#define STBI_NOTUSED(v) (void)sizeof(v)
#endif

#ifdef _MSC_VER
#define STBI_HAS_LROTL
#endif

#ifdef STBI_HAS_LROTL
#define stbi_lrot(x, y) _lrotl(x, y)
#else
#define stbi_lrot(x, y) (((x) << (y)) | ((x) >> (32 - (y))))
#endif

#if defined(STBI_MALLOC) && defined(STBI_FREE) &&                              \
    (defined(STBI_REALLOC) || defined(STBI_REALLOC_SIZED))
// ok
#elif !defined(STBI_MALLOC) && !defined(STBI_FREE) &&                          \
    !defined(STBI_REALLOC) && !defined(STBI_REALLOC_SIZED)
// ok
#else
#error                                                                         \
    "Must define all or none of STBI_MALLOC, STBI_FREE, and STBI_REALLOC (or STBI_REALLOC_SIZED)."
#endif

#ifndef STBI_MALLOC
#define STBI_MALLOC(sz) malloc(sz)
#define STBI_REALLOC(p, newsz) realloc(p, newsz)
#define STBI_FREE(p) free(p)
#endif

#ifndef STBI_REALLOC_SIZED
#define STBI_REALLOC_SIZED(p, oldsz, newsz) STBI_REALLOC(p, newsz)
#endif

// x86/x64 detection
#if defined(__x86_64__) || defined(_M_X64)
#define STBI__X64_TARGET
#elif defined(__i386) || defined(_M_IX86)
#define STBI__X86_TARGET
#endif

#if defined(__GNUC__) && defined(STBI__X86_TARGET) && !defined(__SSE2__) &&    \
    !defined(STBI_NO_SIMD)
// gcc doesn't support sse2 intrinsics unless you compile with -msse2,
// which in turn means it gets to use SSE2 everywhere. This is unfortunate,
// but previous attempts to provide the SSE2 functions with runtime
// detection caused numerous issues. The way architecture extensions are
// exposed in GCC/Clang is, sadly, not really suited for one-file libs.
// New behavior: if compiled with -msse2, we use SSE2 without any
// detection; if not, we don't use it at all.
#define STBI_NO_SIMD
#endif

#if defined(__MINGW32__) && defined(STBI__X86_TARGET) &&                       \
    !defined(STBI_MINGW_ENABLE_SSE2) && !defined(STBI_NO_SIMD)
// Note that __MINGW32__ doesn't actually mean 32-bit, so we have to avoid
// STBI__X64_TARGET
//
// 32-bit MinGW wants ESP to be 16-byte aligned, but this is not in the
// Windows ABI and VC++ as well as Windows DLLs don't maintain that invariant.
// As a result, enabling SSE2 on 32-bit MinGW is dangerous when not
// simultaneously enabling "-mstackrealign".
//
// See https://github.com/nothings/stb/issues/81 for more information.
//
// So default to no SSE2 on 32-bit MinGW. If you've read this far and added
// -mstackrealign to your build settings, feel free to #define
// STBI_MINGW_ENABLE_SSE2.
#define STBI_NO_SIMD
#endif

#if !defined(STBI_NO_SIMD) &&                                                  \
    (defined(STBI__X86_TARGET) || defined(STBI__X64_TARGET))
#define STBI_SSE2
#include <emmintrin.h>

#ifdef _MSC_VER

#if _MSC_VER >= 1400 // not VC6
#include <intrin.h>  // __cpuid
static int stbi__cpuid3(void) {
  int info[4];
  __cpuid(info, 1);
  return info[3];
}
#else
static int stbi__cpuid3(void) {
  int res;
  __asm {
      mov  eax,1
      cpuid
      mov  res,edx
  }
  return res;
}
#endif

#define STBI_SIMD_ALIGN(type, name) __declspec(align(16)) type name

static int stbi__sse2_available(void) {
  int info3 = stbi__cpuid3();
  return ((info3 >> 26) & 1) != 0;
}
#else // assume GCC-style if not VC++
#define STBI_SIMD_ALIGN(type, name) type name __attribute__((aligned(16)))

static int stbi__sse2_available(void) {
  // If we're even attempting to compile this on GCC/Clang, that means
  // -msse2 is on, which means the compiler is allowed to use SSE2
  // instructions at will, and so are we.
  return 1;
}
#endif
#endif

// ARM NEON
#if defined(STBI_NO_SIMD) && defined(STBI_NEON)
#undef STBI_NEON
#endif

#ifdef STBI_NEON
#include <arm_neon.h>
// assume GCC or Clang on ARM targets
#define STBI_SIMD_ALIGN(type, name) type name __attribute__((aligned(16)))
#endif

#ifndef STBI_SIMD_ALIGN
#define STBI_SIMD_ALIGN(type, name) type name
#endif

///////////////////////////////////////////////
//
//  stbi__context struct and start_xxx functions

// stbi__context structure is our basic context used by all images, so it
// contains all the IO context, plus some basic image information
typedef struct {
  stbi__uint32 img_x, img_y;
  int img_n, img_out_n;

  stbi_io_callbacks io;
  void *io_user_data;

  int read_from_callbacks;
  int buflen;
  stbi_uc buffer_start[128];

  stbi_uc *img_buffer, *img_buffer_end;
  stbi_uc *img_buffer_original, *img_buffer_original_end;
} stbi__context;

static void stbi__refill_buffer(stbi__context *s);

// initialize a memory-decode context
static void stbi__start_mem(stbi__context *s, stbi_uc const *buffer, int len) {
  s->io.read = NULL;
  s->read_from_callbacks = 0;
  s->img_buffer = s->img_buffer_original = (stbi_uc *)buffer;
  s->img_buffer_end = s->img_buffer_original_end = (stbi_uc *)buffer + len;
}

// initialize a callback-based context
static void stbi__start_callbacks(stbi__context *s, stbi_io_callbacks *c,
                                  void *user) {
  s->io = *c;
  s->io_user_data = user;
  s->buflen = sizeof(s->buffer_start);
  s->read_from_callbacks = 1;
  s->img_buffer_original = s->buffer_start;
  stbi__refill_buffer(s);
  s->img_buffer_original_end = s->img_buffer_end;
}

#ifndef STBI_NO_STDIO

static int stbi__stdio_read(void *user, char *data, int size) {
  return (int)fread(data, 1, size, (FILE *)user);
}

static void stbi__stdio_skip(void *user, int n) {
  fseek((FILE *)user, n, SEEK_CUR);
}

static int stbi__stdio_eof(void *user) { return feof((FILE *)user); }

static stbi_io_callbacks stbi__stdio_callbacks = {
    stbi__stdio_read,
    stbi__stdio_skip,
    stbi__stdio_eof,
};

static void stbi__start_file(stbi__context *s, FILE *f) {
  stbi__start_callbacks(s, &stbi__stdio_callbacks, (void *)f);
}

  // static void stop_file(stbi__context *s) { }

#endif // !STBI_NO_STDIO

static void stbi__rewind(stbi__context *s) {
  // conceptually rewind SHOULD rewind to the beginning of the stream,
  // but we just rewind to the beginning of the initial buffer, because
  // we only use it after doing 'test', which only ever looks at at most 92
  // bytes
  s->img_buffer = s->img_buffer_original;
  s->img_buffer_end = s->img_buffer_original_end;
}

enum { STBI_ORDER_RGB, STBI_ORDER_BGR };

typedef struct {
  int bits_per_channel;
  int num_channels;
  int channel_order;
} stbi__result_info;

#ifndef STBI_NO_JPEG
static int stbi__jpeg_test(stbi__context *s);
static void *stbi__jpeg_load(stbi__context *s, int *x, int *y, int *comp,
                             int req_comp, stbi__result_info *ri);
static int stbi__jpeg_info(stbi__context *s, int *x, int *y, int *comp);
#endif

#ifndef STBI_NO_PNG
static int stbi__png_test(stbi__context *s);
static void *stbi__png_load(stbi__context *s, int *x, int *y, int *comp,
                            int req_comp, stbi__result_info *ri);
static int stbi__png_info(stbi__context *s, int *x, int *y, int *comp);
#endif

#ifndef STBI_NO_BMP
static int stbi__bmp_test(stbi__context *s);
static void *stbi__bmp_load(stbi__context *s, int *x, int *y, int *comp,
                            int req_comp, stbi__result_info *ri);
static int stbi__bmp_info(stbi__context *s, int *x, int *y, int *comp);
#endif

#ifndef STBI_NO_TGA
static int stbi__tga_test(stbi__context *s);
static void *stbi__tga_load(stbi__context *s, int *x, int *y, int *comp,
                            int req_comp, stbi__result_info *ri);
static int stbi__tga_info(stbi__context *s, int *x, int *y, int *comp);
#endif

#ifndef STBI_NO_PSD
static int stbi__psd_test(stbi__context *s);
static void *stbi__psd_load(stbi__context *s, int *x, int *y, int *comp,
                            int req_comp, stbi__result_info *ri, int bpc);
static int stbi__psd_info(stbi__context *s, int *x, int *y, int *comp);
#endif

#ifndef STBI_NO_HDR
static int stbi__hdr_test(stbi__context *s);
static float *stbi__hdr_load(stbi__context *s, int *x, int *y, int *comp,
                             int req_comp, stbi__result_info *ri);
static int stbi__hdr_info(stbi__context *s, int *x, int *y, int *comp);
#endif

#ifndef STBI_NO_PIC
static int stbi__pic_test(stbi__context *s);
static void *stbi__pic_load(stbi__context *s, int *x, int *y, int *comp,
                            int req_comp, stbi__result_info *ri);
static int stbi__pic_info(stbi__context *s, int *x, int *y, int *comp);
#endif

#ifndef STBI_NO_GIF
static int stbi__gif_test(stbi__context *s);
static void *stbi__gif_load(stbi__context *s, int *x, int *y, int *comp,
                            int req_comp, stbi__result_info *ri);
static int stbi__gif_info(stbi__context *s, int *x, int *y, int *comp);
#endif

#ifndef STBI_NO_PNM
static int stbi__pnm_test(stbi__context *s);
static void *stbi__pnm_load(stbi__context *s, int *x, int *y, int *comp,
                            int req_comp, stbi__result_info *ri);
static int stbi__pnm_info(stbi__context *s, int *x, int *y, int *comp);
#endif

// this is not threadsafe
static const char *stbi__g_failure_reason;

STBIDEF const char *stbi_failure_reason(void) { return stbi__g_failure_reason; }

static int stbi__err(const char *str) {
  stbi__g_failure_reason = str;
  return 0;
}

static void *stbi__malloc(size_t size) { return STBI_MALLOC(size); }

// stb_image uses ints pervasively, including for offset calculations.
// therefore the largest decoded image size we can support with the
// current code, even on 64-bit targets, is INT_MAX. this is not a
// significant limitation for the intended use case.
//
// we do, however, need to make sure our size calculations don't
// overflow. hence a few helper functions for size calculations that
// multiply integers together, making sure that they're non-negative
// and no overflow occurs.

// return 1 if the sum is valid, 0 on overflow.
// negative terms are considered invalid.
static int stbi__addsizes_valid(int a, int b) {
  if (b < 0)
    return 0;
  // now 0 <= b <= INT_MAX, hence also
  // 0 <= INT_MAX - b <= INTMAX.
  // And "a + b <= INT_MAX" (which might overflow) is the
  // same as a <= INT_MAX - b (no overflow)
  return a <= INT_MAX - b;
}

// returns 1 if the product is valid, 0 on overflow.
// negative factors are considered invalid.
static int stbi__mul2sizes_valid(int a, int b) {
  if (a < 0 || b < 0)
    return 0;
  if (b == 0)
    return 1; // mul-by-0 is always safe
  // portable way to check for no overflows in a*b
  return a <= INT_MAX / b;
}

// returns 1 if "a*b + add" has no negative terms/factors and doesn't overflow
static int stbi__mad2sizes_valid(int a, int b, int add) {
  return stbi__mul2sizes_valid(a, b) && stbi__addsizes_valid(a * b, add);
}

// returns 1 if "a*b*c + add" has no negative terms/factors and doesn't overflow
static int stbi__mad3sizes_valid(int a, int b, int c, int add) {
  return stbi__mul2sizes_valid(a, b) && stbi__mul2sizes_valid(a * b, c) &&
         stbi__addsizes_valid(a * b * c, add);
}

// returns 1 if "a*b*c*d + add" has no negative terms/factors and doesn't
// overflow
static int stbi__mad4sizes_valid(int a, int b, int c, int d, int add) {
  return stbi__mul2sizes_valid(a, b) && stbi__mul2sizes_valid(a * b, c) &&
         stbi__mul2sizes_valid(a * b * c, d) &&
         stbi__addsizes_valid(a * b * c * d, add);
}

// mallocs with size overflow checking
static void *stbi__malloc_mad2(int a, int b, int add) {
  if (!stbi__mad2sizes_valid(a, b, add))
    return NULL;
  return stbi__malloc(a * b + add);
}

static void *stbi__malloc_mad3(int a, int b, int c, int add) {
  if (!stbi__mad3sizes_valid(a, b, c, add))
    return NULL;
  return stbi__malloc(a * b * c + add);
}

static void *stbi__malloc_mad4(int a, int b, int c, int d, int add) {
  if (!stbi__mad4sizes_valid(a, b, c, d, add))
    return NULL;
  return stbi__malloc(a * b * c * d + add);
}

  // stbi__err - error
  // stbi__errpf - error returning pointer to float
  // stbi__errpuc - error returning pointer to unsigned char

#ifdef STBI_NO_FAILURE_STRINGS
#define stbi__err(x, y) 0
#elif defined(STBI_FAILURE_USERMSG)
#define stbi__err(x, y) stbi__err(y)
#else
#define stbi__err(x, y) stbi__err(x)
#endif

#define stbi__errpf(x, y) ((float *)(size_t)(stbi__err(x, y) ? NULL : NULL))
#define stbi__errpuc(x, y)                                                     \
  ((unsigned char *)(size_t)(stbi__err(x, y) ? NULL : NULL))

STBIDEF void stbi_image_free(void *retval_from_stbi_load) {
  STBI_FREE(retval_from_stbi_load);
}

#ifndef STBI_NO_LINEAR
static float *stbi__ldr_to_hdr(stbi_uc *data, int x, int y, int comp);
#endif

#ifndef STBI_NO_HDR
static stbi_uc *stbi__hdr_to_ldr(float *data, int x, int y, int comp);
#endif

static int stbi__vertically_flip_on_load = 0;

STBIDEF void stbi_set_flip_vertically_on_load(int flag_true_if_should_flip) {
  stbi__vertically_flip_on_load = flag_true_if_should_flip;
}

static void *stbi__load_main(stbi__context *s, int *x, int *y, int *comp,
                             int req_comp, stbi__result_info *ri, int bpc) {
  memset(ri, 0, sizeof(*ri)); // make sure it's initialized if we add new fields
  ri->bits_per_channel =
      8; // default is 8 so most paths don't have to be changed
  ri->channel_order = STBI_ORDER_RGB; // all current input & output are this,
                                      // but this is here so we can add BGR
                                      // order
  ri->num_channels = 0;

#ifndef STBI_NO_JPEG
  if (stbi__jpeg_test(s))
    return stbi__jpeg_load(s, x, y, comp, req_comp, ri);
#endif
#ifndef STBI_NO_PNG
  if (stbi__png_test(s))
    return stbi__png_load(s, x, y, comp, req_comp, ri);
#endif
#ifndef STBI_NO_BMP
  if (stbi__bmp_test(s))
    return stbi__bmp_load(s, x, y, comp, req_comp, ri);
#endif
#ifndef STBI_NO_GIF
  if (stbi__gif_test(s))
    return stbi__gif_load(s, x, y, comp, req_comp, ri);
#endif
#ifndef STBI_NO_PSD
  if (stbi__psd_test(s))
    return stbi__psd_load(s, x, y, comp, req_comp, ri, bpc);
#endif
#ifndef STBI_NO_PIC
  if (stbi__pic_test(s))
    return stbi__pic_load(s, x, y, comp, req_comp, ri);
#endif
#ifndef STBI_NO_PNM
  if (stbi__pnm_test(s))
    return stbi__pnm_load(s, x, y, comp, req_comp, ri);
#endif

#ifndef STBI_NO_HDR
  if (stbi__hdr_test(s)) {
    float *hdr = stbi__hdr_load(s, x, y, comp, req_comp, ri);
    return stbi__hdr_to_ldr(hdr, *x, *y, req_comp ? req_comp : *comp);
  }
#endif

#ifndef STBI_NO_TGA
  // test tga last because it's a crappy test!
  if (stbi__tga_test(s))
    return stbi__tga_load(s, x, y, comp, req_comp, ri);
#endif

  return stbi__errpuc("unknown image type",
                      "Image not of any known type, or corrupt");
}

static stbi_uc *stbi__convert_16_to_8(stbi__uint16 *orig, int w, int h,
                                      int channels) {
  int i;
  int img_len = w * h * channels;
  stbi_uc *reduced;

  reduced = (stbi_uc *)stbi__malloc(img_len);
  if (reduced == NULL)
    return stbi__errpuc("outofmem", "Out of memory");

  for (i = 0; i < img_len; ++i)
    reduced[i] = (stbi_uc)((orig[i] >> 8) & 0xFF); // top half of each byte is
                                                   // sufficient approx of 16->8
                                                   // bit scaling

  STBI_FREE(orig);
  return reduced;
}

static stbi__uint16 *stbi__convert_8_to_16(stbi_uc *orig, int w, int h,
                                           int channels) {
  int i;
  int img_len = w * h * channels;
  stbi__uint16 *enlarged;

  enlarged = (stbi__uint16 *)stbi__malloc(img_len * 2);
  if (enlarged == NULL)
    return (stbi__uint16 *)stbi__errpuc("outofmem", "Out of memory");

  for (i = 0; i < img_len; ++i)
    enlarged[i] = (stbi__uint16)(
        (orig[i] << 8) +
        orig[i]); // replicate to high and low byte, maps 0->0, 255->0xffff

  STBI_FREE(orig);
  return enlarged;
}

static void stbi__vertical_flip(void *image, int w, int h,
                                int bytes_per_pixel) {
  int row;
  size_t bytes_per_row = (size_t)w * bytes_per_pixel;
  stbi_uc temp[2048];
  stbi_uc *bytes = (stbi_uc *)image;

  for (row = 0; row < (h >> 1); row++) {
    stbi_uc *row0 = bytes + row * bytes_per_row;
    stbi_uc *row1 = bytes + (h - row - 1) * bytes_per_row;
    // swap row0 with row1
    size_t bytes_left = bytes_per_row;
    while (bytes_left) {
      size_t bytes_copy =
          (bytes_left < sizeof(temp)) ? bytes_left : sizeof(temp);
      memcpy(temp, row0, bytes_copy);
      memcpy(row0, row1, bytes_copy);
      memcpy(row1, temp, bytes_copy);
      row0 += bytes_copy;
      row1 += bytes_copy;
      bytes_left -= bytes_copy;
    }
  }
}

static unsigned char *stbi__load_and_postprocess_8bit(stbi__context *s, int *x,
                                                      int *y, int *comp,
                                                      int req_comp) {
  stbi__result_info ri;
  void *result = stbi__load_main(s, x, y, comp, req_comp, &ri, 8);

  if (result == NULL)
    return NULL;

  if (ri.bits_per_channel != 8) {
    STBI_ASSERT(ri.bits_per_channel == 16);
    result = stbi__convert_16_to_8((stbi__uint16 *)result, *x, *y,
                                   req_comp == 0 ? *comp : req_comp);
    ri.bits_per_channel = 8;
  }

  // @TODO: move stbi__convert_format to here

  if (stbi__vertically_flip_on_load) {
    int channels = req_comp ? req_comp : *comp;
    stbi__vertical_flip(result, *x, *y, channels * sizeof(stbi_uc));
  }

  return (unsigned char *)result;
}

static stbi__uint16 *stbi__load_and_postprocess_16bit(stbi__context *s, int *x,
                                                      int *y, int *comp,
                                                      int req_comp) {
  stbi__result_info ri;
  void *result = stbi__load_main(s, x, y, comp, req_comp, &ri, 16);

  if (result == NULL)
    return NULL;

  if (ri.bits_per_channel != 16) {
    STBI_ASSERT(ri.bits_per_channel == 8);
    result = stbi__convert_8_to_16((stbi_uc *)result, *x, *y,
                                   req_comp == 0 ? *comp : req_comp);
    ri.bits_per_channel = 16;
  }

  // @TODO: move stbi__convert_format16 to here
  // @TODO: special case RGB-to-Y (and RGBA-to-YA) for 8-bit-to-16-bit case to
  // keep more precision

  if (stbi__vertically_flip_on_load) {
    int channels = req_comp ? req_comp : *comp;
    stbi__vertical_flip(result, *x, *y, channels * sizeof(stbi__uint16));
  }

  return (stbi__uint16 *)result;
}

#ifndef STBI_NO_HDR
static void stbi__float_postprocess(float *result, int *x, int *y, int *comp,
                                    int req_comp) {
  if (stbi__vertically_flip_on_load && result != NULL) {
    int channels = req_comp ? req_comp : *comp;
    stbi__vertical_flip(result, *x, *y, channels * sizeof(float));
  }
}
#endif

#ifndef STBI_NO_STDIO

static FILE *stbi__fopen(char const *filename, char const *mode) {
  FILE *f;
#if defined(_MSC_VER) && _MSC_VER >= 1400
  if (0 != fopen_s(&f, filename, mode))
    f = 0;
#else
  f = fopen(filename, mode);
#endif
  return f;
}

STBIDEF stbi_uc *stbi_load(char const *filename, int *x, int *y, int *comp,
                           int req_comp) {
  FILE *f = stbi__fopen(filename, "rb");
  unsigned char *result;
  if (!f)
    return stbi__errpuc("can't fopen", "Unable to open file");
  result = stbi_load_from_file(f, x, y, comp, req_comp);
  fclose(f);
  return result;
}

STBIDEF stbi_uc *stbi_load_from_file(FILE *f, int *x, int *y, int *comp,
                                     int req_comp) {
  unsigned char *result;
  stbi__context s;
  stbi__start_file(&s, f);
  result = stbi__load_and_postprocess_8bit(&s, x, y, comp, req_comp);
  if (result) {
    // need to 'unget' all the characters in the IO buffer
    fseek(f, -(int)(s.img_buffer_end - s.img_buffer), SEEK_CUR);
  }
  return result;
}

STBIDEF stbi__uint16 *stbi_load_from_file_16(FILE *f, int *x, int *y, int *comp,
                                             int req_comp) {
  stbi__uint16 *result;
  stbi__context s;
  stbi__start_file(&s, f);
  result = stbi__load_and_postprocess_16bit(&s, x, y, comp, req_comp);
  if (result) {
    // need to 'unget' all the characters in the IO buffer
    fseek(f, -(int)(s.img_buffer_end - s.img_buffer), SEEK_CUR);
  }
  return result;
}

STBIDEF stbi_us *stbi_load_16(char const *filename, int *x, int *y, int *comp,
                              int req_comp) {
  FILE *f = stbi__fopen(filename, "rb");
  stbi__uint16 *result;
  if (!f)
    return (stbi_us *)stbi__errpuc("can't fopen", "Unable to open file");
  result = stbi_load_from_file_16(f, x, y, comp, req_comp);
  fclose(f);
  return result;
}

#endif //! STBI_NO_STDIO

STBIDEF stbi_us *stbi_load_16_from_memory(stbi_uc const *buffer, int len,
                                          int *x, int *y, int *channels_in_file,
                                          int desired_channels) {
  stbi__context s;
  stbi__start_mem(&s, buffer, len);
  return stbi__load_and_postprocess_16bit(&s, x, y, channels_in_file,
                                          desired_channels);
}

STBIDEF stbi_us *stbi_load_16_from_callbacks(stbi_io_callbacks const *clbk,
                                             void *user, int *x, int *y,
                                             int *channels_in_file,
                                             int desired_channels) {
  stbi__context s;
  stbi__start_callbacks(&s, (stbi_io_callbacks *)clbk, user);
  return stbi__load_and_postprocess_16bit(&s, x, y, channels_in_file,
                                          desired_channels);
}

STBIDEF stbi_uc *stbi_load_from_memory(stbi_uc const *buffer, int len, int *x,
                                       int *y, int *comp, int req_comp) {
  stbi__context s;
  stbi__start_mem(&s, buffer, len);
  return stbi__load_and_postprocess_8bit(&s, x, y, comp, req_comp);
}

STBIDEF stbi_uc *stbi_load_from_callbacks(stbi_io_callbacks const *clbk,
                                          void *user, int *x, int *y, int *comp,
                                          int req_comp) {
  stbi__context s;
  stbi__start_callbacks(&s, (stbi_io_callbacks *)clbk, user);
  return stbi__load_and_postprocess_8bit(&s, x, y, comp, req_comp);
}

#ifndef STBI_NO_LINEAR
static float *stbi__loadf_main(stbi__context *s, int *x, int *y, int *comp,
                               int req_comp) {
  unsigned char *data;
#ifndef STBI_NO_HDR
  if (stbi__hdr_test(s)) {
    stbi__result_info ri;
    float *hdr_data = stbi__hdr_load(s, x, y, comp, req_comp, &ri);
    if (hdr_data)
      stbi__float_postprocess(hdr_data, x, y, comp, req_comp);
    return hdr_data;
  }
#endif
  data = stbi__load_and_postprocess_8bit(s, x, y, comp, req_comp);
  if (data)
    return stbi__ldr_to_hdr(data, *x, *y, req_comp ? req_comp : *comp);
  return stbi__errpf("unknown image type",
                     "Image not of any known type, or corrupt");
}

STBIDEF float *stbi_loadf_from_memory(stbi_uc const *buffer, int len, int *x,
                                      int *y, int *comp, int req_comp) {
  stbi__context s;
  stbi__start_mem(&s, buffer, len);
  return stbi__loadf_main(&s, x, y, comp, req_comp);
}

STBIDEF float *stbi_loadf_from_callbacks(stbi_io_callbacks const *clbk,
                                         void *user, int *x, int *y, int *comp,
                                         int req_comp) {
  stbi__context s;
  stbi__start_callbacks(&s, (stbi_io_callbacks *)clbk, user);
  return stbi__loadf_main(&s, x, y, comp, req_comp);
}

#ifndef STBI_NO_STDIO
STBIDEF float *stbi_loadf(char const *filename, int *x, int *y, int *comp,
                          int req_comp) {
  float *result;
  FILE *f = stbi__fopen(filename, "rb");
  if (!f)
    return stbi__errpf("can't fopen", "Unable to open file");
  result = stbi_loadf_from_file(f, x, y, comp, req_comp);
  fclose(f);
  return result;
}

STBIDEF float *stbi_loadf_from_file(FILE *f, int *x, int *y, int *comp,
                                    int req_comp) {
  stbi__context s;
  stbi__start_file(&s, f);
  return stbi__loadf_main(&s, x, y, comp, req_comp);
}
#endif // !STBI_NO_STDIO

#endif // !STBI_NO_LINEAR

// these is-hdr-or-not is defined independent of whether STBI_NO_LINEAR is
// defined, for API simplicity; if STBI_NO_LINEAR is defined, it always
// reports false!

STBIDEF int stbi_is_hdr_from_memory(stbi_uc const *buffer, int len) {
#ifndef STBI_NO_HDR
  stbi__context s;
  stbi__start_mem(&s, buffer, len);
  return stbi__hdr_test(&s);
#else
  STBI_NOTUSED(buffer);
  STBI_NOTUSED(len);
  return 0;
#endif
}

#ifndef STBI_NO_STDIO
STBIDEF int stbi_is_hdr(char const *filename) {
  FILE *f = stbi__fopen(filename, "rb");
  int result = 0;
  if (f) {
    result = stbi_is_hdr_from_file(f);
    fclose(f);
  }
  return result;
}

STBIDEF int stbi_is_hdr_from_file(FILE *f) {
#ifndef STBI_NO_HDR
  stbi__context s;
  stbi__start_file(&s, f);
  return stbi__hdr_test(&s);
#else
  STBI_NOTUSED(f);
  return 0;
#endif
}
#endif // !STBI_NO_STDIO

STBIDEF int stbi_is_hdr_from_callbacks(stbi_io_callbacks const *clbk,
                                       void *user) {
#ifndef STBI_NO_HDR
  stbi__context s;
  stbi__start_callbacks(&s, (stbi_io_callbacks *)clbk, user);
  return stbi__hdr_test(&s);
#else
  STBI_NOTUSED(clbk);
  STBI_NOTUSED(user);
  return 0;
#endif
}

#ifndef STBI_NO_LINEAR
static float stbi__l2h_gamma = 2.2f, stbi__l2h_scale = 1.0f;

STBIDEF void stbi_ldr_to_hdr_gamma(float gamma) { stbi__l2h_gamma = gamma; }
STBIDEF void stbi_ldr_to_hdr_scale(float scale) { stbi__l2h_scale = scale; }
#endif

static float stbi__h2l_gamma_i = 1.0f / 2.2f, stbi__h2l_scale_i = 1.0f;

STBIDEF void stbi_hdr_to_ldr_gamma(float gamma) {
  stbi__h2l_gamma_i = 1 / gamma;
}
STBIDEF void stbi_hdr_to_ldr_scale(float scale) {
  stbi__h2l_scale_i = 1 / scale;
}

//////////////////////////////////////////////////////////////////////////////
//
// Common code used by all image loaders
//

enum { STBI__SCAN_load = 0, STBI__SCAN_type, STBI__SCAN_header };

static void stbi__refill_buffer(stbi__context *s) {
  int n = (s->io.read)(s->io_user_data, (char *)s->buffer_start, s->buflen);
  if (n == 0) {
    // at end of file, treat same as if from memory, but need to handle case
    // where s->img_buffer isn't pointing to safe memory, e.g. 0-byte file
    s->read_from_callbacks = 0;
    s->img_buffer = s->buffer_start;
    s->img_buffer_end = s->buffer_start + 1;
    *s->img_buffer = 0;
  } else {
    s->img_buffer = s->buffer_start;
    s->img_buffer_end = s->buffer_start + n;
  }
}

stbi_inline static stbi_uc stbi__get8(stbi__context *s) {
  if (s->img_buffer < s->img_buffer_end)
    return *s->img_buffer++;
  if (s->read_from_callbacks) {
    stbi__refill_buffer(s);
    return *s->img_buffer++;
  }
  return 0;
}

stbi_inline static int stbi__at_eof(stbi__context *s) {
  if (s->io.read) {
    if (!(s->io.eof)(s->io_user_data))
      return 0;
    // if feof() is true, check if buffer = end
    // special case: we've only got the special 0 character at the end
    if (s->read_from_callbacks == 0)
      return 1;
  }

  return s->img_buffer >= s->img_buffer_end;
}

static void stbi__skip(stbi__context *s, int n) {
  if (n < 0) {
    s->img_buffer = s->img_buffer_end;
    return;
  }
  if (s->io.read) {
    int blen = (int)(s->img_buffer_end - s->img_buffer);
    if (blen < n) {
      s->img_buffer = s->img_buffer_end;
      (s->io.skip)(s->io_user_data, n - blen);
      return;
    }
  }
  s->img_buffer += n;
}

static int stbi__getn(stbi__context *s, stbi_uc *buffer, int n) {
  if (s->io.read) {
    int blen = (int)(s->img_buffer_end - s->img_buffer);
    if (blen < n) {
      int res, count;

      memcpy(buffer, s->img_buffer, blen);

      count = (s->io.read)(s->io_user_data, (char *)buffer + blen, n - blen);
      res = (count == (n - blen));
      s->img_buffer = s->img_buffer_end;
      return res;
    }
  }

  if (s->img_buffer + n <= s->img_buffer_end) {
    memcpy(buffer, s->img_buffer, n);
    s->img_buffer += n;
    return 1;
  } else
    return 0;
}

static int stbi__get16be(stbi__context *s) {
  int z = stbi__get8(s);
  return (z << 8) + stbi__get8(s);
}

static stbi__uint32 stbi__get32be(stbi__context *s) {
  stbi__uint32 z = stbi__get16be(s);
  return (z << 16) + stbi__get16be(s);
}

#if defined(STBI_NO_BMP) && defined(STBI_NO_TGA) && defined(STBI_NO_GIF)
// nothing
#else
static int stbi__get16le(stbi__context *s) {
  int z = stbi__get8(s);
  return z + (stbi__get8(s) << 8);
}
#endif

#ifndef STBI_NO_BMP
static stbi__uint32 stbi__get32le(stbi__context *s) {
  stbi__uint32 z = stbi__get16le(s);
  return z + (stbi__get16le(s) << 16);
}
#endif

#define STBI__BYTECAST(x)                                                      \
  ((stbi_uc)((x)&255)) // truncate int to byte without warnings

//////////////////////////////////////////////////////////////////////////////
//
//  generic converter from built-in img_n to req_comp
//    individual types do this automatically as much as possible (e.g. jpeg
//    does all cases internally since it needs to colorspace convert anyway,
//    and it never has alpha, so very few cases ). png can automatically
//    interleave an alpha=255 channel, but falls back to this for other cases
//
//  assume data buffer is malloced, so malloc a new one and free that one
//  only failure mode is malloc failing

static stbi_uc stbi__compute_y(int r, int g, int b) {
  return (stbi_uc)(((r * 77) + (g * 150) + (29 * b)) >> 8);
}

static unsigned char *stbi__convert_format(unsigned char *data, int img_n,
                                           int req_comp, unsigned int x,
                                           unsigned int y) {
  int i, j;
  unsigned char *good;

  if (req_comp == img_n)
    return data;
  STBI_ASSERT(req_comp >= 1 && req_comp <= 4);

  good = (unsigned char *)stbi__malloc_mad3(req_comp, x, y, 0);
  if (good == NULL) {
    STBI_FREE(data);
    return stbi__errpuc("outofmem", "Out of memory");
  }

  for (j = 0; j < (int)y; ++j) {
    unsigned char *src = data + j * x * img_n;
    unsigned char *dest = good + j * x * req_comp;

#define STBI__COMBO(a, b) ((a)*8 + (b))
#define STBI__CASE(a, b)                                                       \
  case STBI__COMBO(a, b):                                                      \
    for (i = x - 1; i >= 0; --i, src += a, dest += b)
    // convert source image with img_n components to one with req_comp
    // components; avoid switch per pixel, so use switch per scanline and
    // massive macros
    switch (STBI__COMBO(img_n, req_comp)) {
      STBI__CASE(1, 2) { dest[0] = src[0], dest[1] = 255; }
      break;
      STBI__CASE(1, 3) { dest[0] = dest[1] = dest[2] = src[0]; }
      break;
      STBI__CASE(1, 4) { dest[0] = dest[1] = dest[2] = src[0], dest[3] = 255; }
      break;
      STBI__CASE(2, 1) { dest[0] = src[0]; }
      break;
      STBI__CASE(2, 3) { dest[0] = dest[1] = dest[2] = src[0]; }
      break;
      STBI__CASE(2, 4) {
        dest[0] = dest[1] = dest[2] = src[0], dest[3] = src[1];
      }
      break;
      STBI__CASE(3, 4) {
        dest[0] = src[0], dest[1] = src[1], dest[2] = src[2], dest[3] = 255;
      }
      break;
      STBI__CASE(3, 1) { dest[0] = stbi__compute_y(src[0], src[1], src[2]); }
      break;
      STBI__CASE(3, 2) {
        dest[0] = stbi__compute_y(src[0], src[1], src[2]), dest[1] = 255;
      }
      break;
      STBI__CASE(4, 1) { dest[0] = stbi__compute_y(src[0], src[1], src[2]); }
      break;
      STBI__CASE(4, 2) {
        dest[0] = stbi__compute_y(src[0], src[1], src[2]), dest[1] = src[3];
      }
      break;
      STBI__CASE(4, 3) { dest[0] = src[0], dest[1] = src[1], dest[2] = src[2]; }
      break;
    default:
      STBI_ASSERT(0);
    }
#undef STBI__CASE
  }

  STBI_FREE(data);
  return good;
}

static stbi__uint16 stbi__compute_y_16(int r, int g, int b) {
  return (stbi__uint16)(((r * 77) + (g * 150) + (29 * b)) >> 8);
}

static stbi__uint16 *stbi__convert_format16(stbi__uint16 *data, int img_n,
                                            int req_comp, unsigned int x,
                                            unsigned int y) {
  int i, j;
  stbi__uint16 *good;

  if (req_comp == img_n)
    return data;
  STBI_ASSERT(req_comp >= 1 && req_comp <= 4);

  good = (stbi__uint16 *)stbi__malloc(req_comp * x * y * 2);
  if (good == NULL) {
    STBI_FREE(data);
    return (stbi__uint16 *)stbi__errpuc("outofmem", "Out of memory");
  }

  for (j = 0; j < (int)y; ++j) {
    stbi__uint16 *src = data + j * x * img_n;
    stbi__uint16 *dest = good + j * x * req_comp;

#define STBI__COMBO(a, b) ((a)*8 + (b))
#define STBI__CASE(a, b)                                                       \
  case STBI__COMBO(a, b):                                                      \
    for (i = x - 1; i >= 0; --i, src += a, dest += b)
    // convert source image with img_n components to one with req_comp
    // components; avoid switch per pixel, so use switch per scanline and
    // massive macros
    switch (STBI__COMBO(img_n, req_comp)) {
      STBI__CASE(1, 2) { dest[0] = src[0], dest[1] = 0xffff; }
      break;
      STBI__CASE(1, 3) { dest[0] = dest[1] = dest[2] = src[0]; }
      break;
      STBI__CASE(1, 4) {
        dest[0] = dest[1] = dest[2] = src[0], dest[3] = 0xffff;
      }
      break;
      STBI__CASE(2, 1) { dest[0] = src[0]; }
      break;
      STBI__CASE(2, 3) { dest[0] = dest[1] = dest[2] = src[0]; }
      break;
      STBI__CASE(2, 4) {
        dest[0] = dest[1] = dest[2] = src[0], dest[3] = src[1];
      }
      break;
      STBI__CASE(3, 4) {
        dest[0] = src[0], dest[1] = src[1], dest[2] = src[2], dest[3] = 0xffff;
      }
      break;
      STBI__CASE(3, 1) { dest[0] = stbi__compute_y_16(src[0], src[1], src[2]); }
      break;
      STBI__CASE(3, 2) {
        dest[0] = stbi__compute_y_16(src[0], src[1], src[2]), dest[1] = 0xffff;
      }
      break;
      STBI__CASE(4, 1) { dest[0] = stbi__compute_y_16(src[0], src[1], src[2]); }
      break;
      STBI__CASE(4, 2) {
        dest[0] = stbi__compute_y_16(src[0], src[1], src[2]), dest[1] = src[3];
      }
      break;
      STBI__CASE(4, 3) { dest[0] = src[0], dest[1] = src[1], dest[2] = src[2]; }
      break;
    default:
      STBI_ASSERT(0);
    }
#undef STBI__CASE
  }

  STBI_FREE(data);
  return good;
}

#ifndef STBI_NO_LINEAR
static float *stbi__ldr_to_hdr(stbi_uc *data, int x, int y, int comp) {
  int i, k, n;
  float *output;
  if (!data)
    return NULL;
  output = (float *)stbi__malloc_mad4(x, y, comp, sizeof(float), 0);
  if (output == NULL) {
    STBI_FREE(data);
    return stbi__errpf("outofmem", "Out of memory");
  }
  // compute number of non-alpha components
  if (comp & 1)
    n = comp;
  else
    n = comp - 1;
  for (i = 0; i < x * y; ++i) {
    for (k = 0; k < n; ++k) {
      output[i * comp + k] =
          (float)(pow(data[i * comp + k] / 255.0f, stbi__l2h_gamma) *
                  stbi__l2h_scale);
    }
    if (k < comp)
      output[i * comp + k] = data[i * comp + k] / 255.0f;
  }
  STBI_FREE(data);
  return output;
}
#endif

#ifndef STBI_NO_HDR
#define stbi__float2int(x) ((int)(x))
static stbi_uc *stbi__hdr_to_ldr(float *data, int x, int y, int comp) {
  int i, k, n;
  stbi_uc *output;
  if (!data)
    return NULL;
  output = (stbi_uc *)stbi__malloc_mad3(x, y, comp, 0);
  if (output == NULL) {
    STBI_FREE(data);
    return stbi__errpuc("outofmem", "Out of memory");
  }
  // compute number of non-alpha components
  if (comp & 1)
    n = comp;
  else
    n = comp - 1;
  for (i = 0; i < x * y; ++i) {
    for (k = 0; k < n; ++k) {
      float z = (float)pow(data[i * comp + k] * stbi__h2l_scale_i,
                           stbi__h2l_gamma_i) *
                    255 +
                0.5f;
      if (z < 0)
        z = 0;
      if (z > 255)
        z = 255;
      output[i * comp + k] = (stbi_uc)stbi__float2int(z);
    }
    if (k < comp) {
      float z = data[i * comp + k] * 255 + 0.5f;
      if (z < 0)
        z = 0;
      if (z > 255)
        z = 255;
      output[i * comp + k] = (stbi_uc)stbi__float2int(z);
    }
  }
  STBI_FREE(data);
  return output;
}
#endif

  //////////////////////////////////////////////////////////////////////////////
  //
  //  "baseline" JPEG/JFIF decoder
  //
  //    simple implementation
  //      - doesn't support delayed output of y-dimension
  //      - simple interface (only one output format: 8-bit interleaved RGB)
  //      - doesn't try to recover corrupt jpegs
  //      - doesn't allow partial loading, loading multiple at once
  //      - still fast on x86 (copying globals into locals doesn't help x86)
  //      - allocates lots of intermediate memory (full size of all components)
  //        - non-interleaved case requires this anyway
  //        - allows good upsampling (see next)
  //    high-quality
  //      - upsampled channels are bilinearly interpolated, even across blocks
  //      - quality integer IDCT derived from IJG's 'slow'
  //    performance
  //      - fast huffman; reasonable integer IDCT
  //      - some SIMD kernels for common paths on targets with SSE2/NEON
  //      - uses a lot of intermediate memory, could cache poorly

#ifndef STBI_NO_JPEG

// huffman decoding acceleration
#define FAST_BITS 9 // larger handles more cases; smaller stomps less cache

typedef struct {
  stbi_uc fast[1 << FAST_BITS];
  // weirdly, repacking this into AoS is a 10% speed loss, instead of a win
  stbi__uint16 code[256];
  stbi_uc values[256];
  stbi_uc size[257];
  unsigned int maxcode[18];
  int delta[17]; // old 'firstsymbol' - old 'firstcode'
} stbi__huffman;

typedef struct {
  stbi__context *s;
  stbi__huffman huff_dc[4];
  stbi__huffman huff_ac[4];
  stbi__uint16 dequant[4][64];
  stbi__int16 fast_ac[4][1 << FAST_BITS];

  // sizes for components, interleaved MCUs
  int img_h_max, img_v_max;
  int img_mcu_x, img_mcu_y;
  int img_mcu_w, img_mcu_h;

  // definition of jpeg image component
  struct {
    int id;
    int h, v;
    int tq;
    int hd, ha;
    int dc_pred;

    int x, y, w2, h2;
    stbi_uc *data;
    void *raw_data, *raw_coeff;
    stbi_uc *linebuf;
    short *coeff;         // progressive only
    int coeff_w, coeff_h; // number of 8x8 coefficient blocks
  } img_comp[4];

  stbi__uint32 code_buffer; // jpeg entropy-coded buffer
  int code_bits;            // number of valid bits
  unsigned char marker;     // marker seen while filling entropy buffer
  int nomore;               // flag if we saw a marker so must stop

  int progressive;
  int spec_start;
  int spec_end;
  int succ_high;
  int succ_low;
  int eob_run;
  int jfif;
  int app14_color_transform; // Adobe APP14 tag
  int rgb;

  int scan_n, order[4];
  int restart_interval, todo;

  // kernels
  void (*idct_block_kernel)(stbi_uc *out, int out_stride, short data[64]);
  void (*YCbCr_to_RGB_kernel)(stbi_uc *out, const stbi_uc *y,
                              const stbi_uc *pcb, const stbi_uc *pcr, int count,
                              int step);
  stbi_uc *(*resample_row_hv_2_kernel)(stbi_uc *out, stbi_uc *in_near,
                                       stbi_uc *in_far, int w, int hs);
} stbi__jpeg;

static int stbi__build_huffman(stbi__huffman *h, int *count) {
  int i, j, k = 0, code;
  // build size list for each symbol (from JPEG spec)
  for (i = 0; i < 16; ++i)
    for (j = 0; j < count[i]; ++j)
      h->size[k++] = (stbi_uc)(i + 1);
  h->size[k] = 0;

  // compute actual symbols (from jpeg spec)
  code = 0;
  k = 0;
  for (j = 1; j <= 16; ++j) {
    // compute delta to add to code to compute symbol id
    h->delta[j] = k - code;
    if (h->size[k] == j) {
      while (h->size[k] == j)
        h->code[k++] = (stbi__uint16)(code++);
      if (code - 1 >= (1 << j))
        return stbi__err("bad code lengths", "Corrupt JPEG");
    }
    // compute largest code + 1 for this size, preshifted as needed later
    h->maxcode[j] = code << (16 - j);
    code <<= 1;
  }
  h->maxcode[j] = 0xffffffff;

  // build non-spec acceleration table; 255 is flag for not-accelerated
  memset(h->fast, 255, 1 << FAST_BITS);
  for (i = 0; i < k; ++i) {
    int s = h->size[i];
    if (s <= FAST_BITS) {
      int c = h->code[i] << (FAST_BITS - s);
      int m = 1 << (FAST_BITS - s);
      for (j = 0; j < m; ++j) {
        h->fast[c + j] = (stbi_uc)i;
      }
    }
  }
  return 1;
}

// build a table that decodes both magnitude and value of small ACs in
// one go.
static void stbi__build_fast_ac(stbi__int16 *fast_ac, stbi__huffman *h) {
  int i;
  for (i = 0; i < (1 << FAST_BITS); ++i) {
    stbi_uc fast = h->fast[i];
    fast_ac[i] = 0;
    if (fast < 255) {
      int rs = h->values[fast];
      int run = (rs >> 4) & 15;
      int magbits = rs & 15;
      int len = h->size[fast];

      if (magbits && len + magbits <= FAST_BITS) {
        // magnitude code followed by receive_extend code
        int k = ((i << len) & ((1 << FAST_BITS) - 1)) >> (FAST_BITS - magbits);
        int m = 1 << (magbits - 1);
        if (k < m)
          k += (~0U << magbits) + 1;
        // if the result is small enough, we can fit it in fast_ac table
        if (k >= -128 && k <= 127)
          fast_ac[i] = (stbi__int16)((k << 8) + (run << 4) + (len + magbits));
      }
    }
  }
}

static void stbi__grow_buffer_unsafe(stbi__jpeg *j) {
  do {
    int b = j->nomore ? 0 : stbi__get8(j->s);
    if (b == 0xff) {
      int c = stbi__get8(j->s);
      while (c == 0xff)
        c = stbi__get8(j->s); // consume fill bytes
      if (c != 0) {
        j->marker = (unsigned char)c;
        j->nomore = 1;
        return;
      }
    }
    j->code_buffer |= b << (24 - j->code_bits);
    j->code_bits += 8;
  } while (j->code_bits <= 24);
}

// (1 << n) - 1
static stbi__uint32 stbi__bmask[17] = {0,    1,    3,     7,     15,   31,
                                       63,   127,  255,   511,   1023, 2047,
                                       4095, 8191, 16383, 32767, 65535};

// decode a jpeg huffman value from the bitstream
stbi_inline static int stbi__jpeg_huff_decode(stbi__jpeg *j, stbi__huffman *h) {
  unsigned int temp;
  int c, k;

  if (j->code_bits < 16)
    stbi__grow_buffer_unsafe(j);

  // look at the top FAST_BITS and determine what symbol ID it is,
  // if the code is <= FAST_BITS
  c = (j->code_buffer >> (32 - FAST_BITS)) & ((1 << FAST_BITS) - 1);
  k = h->fast[c];
  if (k < 255) {
    int s = h->size[k];
    if (s > j->code_bits)
      return -1;
    j->code_buffer <<= s;
    j->code_bits -= s;
    return h->values[k];
  }

  // naive test is to shift the code_buffer down so k bits are
  // valid, then test against maxcode. To speed this up, we've
  // preshifted maxcode left so that it has (16-k) 0s at the
  // end; in other words, regardless of the number of bits, it
  // wants to be compared against something shifted to have 16;
  // that way we don't need to shift inside the loop.
  temp = j->code_buffer >> 16;
  for (k = FAST_BITS + 1;; ++k)
    if (temp < h->maxcode[k])
      break;
  if (k == 17) {
    // error! code not found
    j->code_bits -= 16;
    return -1;
  }

  if (k > j->code_bits)
    return -1;

  // convert the huffman code to the symbol id
  c = ((j->code_buffer >> (32 - k)) & stbi__bmask[k]) + h->delta[k];
  STBI_ASSERT((((j->code_buffer) >> (32 - h->size[c])) &
               stbi__bmask[h->size[c]]) == h->code[c]);

  // convert the id to a symbol
  j->code_bits -= k;
  j->code_buffer <<= k;
  return h->values[c];
}

// bias[n] = (-1<<n) + 1
static int const stbi__jbias[16] = {0,     -1,    -3,     -7,    -15,   -31,
                                    -63,   -127,  -255,   -511,  -1023, -2047,
                                    -4095, -8191, -16383, -32767};

// combined JPEG 'receive' and JPEG 'extend', since baseline
// always extends everything it receives.
stbi_inline static int stbi__extend_receive(stbi__jpeg *j, int n) {
  unsigned int k;
  int sgn;
  if (j->code_bits < n)
    stbi__grow_buffer_unsafe(j);

  sgn = (stbi__int32)j->code_buffer >> 31; // sign bit is always in MSB
  k = stbi_lrot(j->code_buffer, n);
  STBI_ASSERT(n >= 0 && n < (int)(sizeof(stbi__bmask) / sizeof(*stbi__bmask)));
  j->code_buffer = k & ~stbi__bmask[n];
  k &= stbi__bmask[n];
  j->code_bits -= n;
  return k + (stbi__jbias[n] & ~sgn);
}

// get some unsigned bits
stbi_inline static int stbi__jpeg_get_bits(stbi__jpeg *j, int n) {
  unsigned int k;
  if (j->code_bits < n)
    stbi__grow_buffer_unsafe(j);
  k = stbi_lrot(j->code_buffer, n);
  j->code_buffer = k & ~stbi__bmask[n];
  k &= stbi__bmask[n];
  j->code_bits -= n;
  return k;
}

stbi_inline static int stbi__jpeg_get_bit(stbi__jpeg *j) {
  unsigned int k;
  if (j->code_bits < 1)
    stbi__grow_buffer_unsafe(j);
  k = j->code_buffer;
  j->code_buffer <<= 1;
  --j->code_bits;
  return k & 0x80000000;
}

// given a value that's at position X in the zigzag stream,
// where does it appear in the 8x8 matrix coded as row-major?
static stbi_uc stbi__jpeg_dezigzag[64 + 15] = {
    0, 1, 8, 16, 9, 2, 3, 10, 17, 24, 32, 25, 18, 11, 4, 5, 12, 19, 26, 33, 40,
    48, 41, 34, 27, 20, 13, 6, 7, 14, 21, 28, 35, 42, 49, 56, 57, 50, 43, 36,
    29, 22, 15, 23, 30, 37, 44, 51, 58, 59, 52, 45, 38, 31, 39, 46, 53, 60, 61,
    54, 47, 55, 62, 63,
    // let corrupt input sample past end
    63, 63, 63, 63, 63, 63, 63, 63, 63, 63, 63, 63, 63, 63, 63};

// decode one 64-entry block--
static int stbi__jpeg_decode_block(stbi__jpeg *j, short data[64],
                                   stbi__huffman *hdc, stbi__huffman *hac,
                                   stbi__int16 *fac, int b,
                                   stbi__uint16 *dequant) {
  int diff, dc, k;
  int t;

  if (j->code_bits < 16)
    stbi__grow_buffer_unsafe(j);
  t = stbi__jpeg_huff_decode(j, hdc);
  if (t < 0)
    return stbi__err("bad huffman code", "Corrupt JPEG");

  // 0 all the ac values now so we can do it 32-bits at a time
  memset(data, 0, 64 * sizeof(data[0]));

  diff = t ? stbi__extend_receive(j, t) : 0;
  dc = j->img_comp[b].dc_pred + diff;
  j->img_comp[b].dc_pred = dc;
  data[0] = (short)(dc * dequant[0]);

  // decode AC components, see JPEG spec
  k = 1;
  do {
    unsigned int zig;
    int c, r, s;
    if (j->code_bits < 16)
      stbi__grow_buffer_unsafe(j);
    c = (j->code_buffer >> (32 - FAST_BITS)) & ((1 << FAST_BITS) - 1);
    r = fac[c];
    if (r) {              // fast-AC path
      k += (r >> 4) & 15; // run
      s = r & 15;         // combined length
      j->code_buffer <<= s;
      j->code_bits -= s;
      // decode into unzigzag'd location
      zig = stbi__jpeg_dezigzag[k++];
      data[zig] = (short)((r >> 8) * dequant[zig]);
    } else {
      int rs = stbi__jpeg_huff_decode(j, hac);
      if (rs < 0)
        return stbi__err("bad huffman code", "Corrupt JPEG");
      s = rs & 15;
      r = rs >> 4;
      if (s == 0) {
        if (rs != 0xf0)
          break; // end block
        k += 16;
      } else {
        k += r;
        // decode into unzigzag'd location
        zig = stbi__jpeg_dezigzag[k++];
        data[zig] = (short)(stbi__extend_receive(j, s) * dequant[zig]);
      }
    }
  } while (k < 64);
  return 1;
}

static int stbi__jpeg_decode_block_prog_dc(stbi__jpeg *j, short data[64],
                                           stbi__huffman *hdc, int b) {
  int diff, dc;
  int t;
  if (j->spec_end != 0)
    return stbi__err("can't merge dc and ac", "Corrupt JPEG");

  if (j->code_bits < 16)
    stbi__grow_buffer_unsafe(j);

  if (j->succ_high == 0) {
    // first scan for DC coefficient, must be first
    memset(data, 0, 64 * sizeof(data[0])); // 0 all the ac values now
    t = stbi__jpeg_huff_decode(j, hdc);
    diff = t ? stbi__extend_receive(j, t) : 0;

    dc = j->img_comp[b].dc_pred + diff;
    j->img_comp[b].dc_pred = dc;
    data[0] = (short)(dc << j->succ_low);
  } else {
    // refinement scan for DC coefficient
    if (stbi__jpeg_get_bit(j))
      data[0] += (short)(1 << j->succ_low);
  }
  return 1;
}

// @OPTIMIZE: store non-zigzagged during the decode passes,
// and only de-zigzag when dequantizing
static int stbi__jpeg_decode_block_prog_ac(stbi__jpeg *j, short data[64],
                                           stbi__huffman *hac,
                                           stbi__int16 *fac) {
  int k;
  if (j->spec_start == 0)
    return stbi__err("can't merge dc and ac", "Corrupt JPEG");

  if (j->succ_high == 0) {
    int shift = j->succ_low;

    if (j->eob_run) {
      --j->eob_run;
      return 1;
    }

    k = j->spec_start;
    do {
      unsigned int zig;
      int c, r, s;
      if (j->code_bits < 16)
        stbi__grow_buffer_unsafe(j);
      c = (j->code_buffer >> (32 - FAST_BITS)) & ((1 << FAST_BITS) - 1);
      r = fac[c];
      if (r) {              // fast-AC path
        k += (r >> 4) & 15; // run
        s = r & 15;         // combined length
        j->code_buffer <<= s;
        j->code_bits -= s;
        zig = stbi__jpeg_dezigzag[k++];
        data[zig] = (short)((r >> 8) << shift);
      } else {
        int rs = stbi__jpeg_huff_decode(j, hac);
        if (rs < 0)
          return stbi__err("bad huffman code", "Corrupt JPEG");
        s = rs & 15;
        r = rs >> 4;
        if (s == 0) {
          if (r < 15) {
            j->eob_run = (1 << r);
            if (r)
              j->eob_run += stbi__jpeg_get_bits(j, r);
            --j->eob_run;
            break;
          }
          k += 16;
        } else {
          k += r;
          zig = stbi__jpeg_dezigzag[k++];
          data[zig] = (short)(stbi__extend_receive(j, s) << shift);
        }
      }
    } while (k <= j->spec_end);
  } else {
    // refinement scan for these AC coefficients

    short bit = (short)(1 << j->succ_low);

    if (j->eob_run) {
      --j->eob_run;
      for (k = j->spec_start; k <= j->spec_end; ++k) {
        short *p = &data[stbi__jpeg_dezigzag[k]];
        if (*p != 0)
          if (stbi__jpeg_get_bit(j))
            if ((*p & bit) == 0) {
              if (*p > 0)
                *p += bit;
              else
                *p -= bit;
            }
      }
    } else {
      k = j->spec_start;
      do {
        int r, s;
        int rs = stbi__jpeg_huff_decode(j, hac); // @OPTIMIZE see if we can use
                                                 // the fast path here,
                                                 // advance-by-r is so slow, eh
        if (rs < 0)
          return stbi__err("bad huffman code", "Corrupt JPEG");
        s = rs & 15;
        r = rs >> 4;
        if (s == 0) {
          if (r < 15) {
            j->eob_run = (1 << r) - 1;
            if (r)
              j->eob_run += stbi__jpeg_get_bits(j, r);
            r = 64; // force end of block
          } else {
            // r=15 s=0 should write 16 0s, so we just do
            // a run of 15 0s and then write s (which is 0),
            // so we don't have to do anything special here
          }
        } else {
          if (s != 1)
            return stbi__err("bad huffman code", "Corrupt JPEG");
          // sign bit
          if (stbi__jpeg_get_bit(j))
            s = bit;
          else
            s = -bit;
        }

        // advance by r
        while (k <= j->spec_end) {
          short *p = &data[stbi__jpeg_dezigzag[k++]];
          if (*p != 0) {
            if (stbi__jpeg_get_bit(j))
              if ((*p & bit) == 0) {
                if (*p > 0)
                  *p += bit;
                else
                  *p -= bit;
              }
          } else {
            if (r == 0) {
              *p = (short)s;
              break;
            }
            --r;
          }
        }
      } while (k <= j->spec_end);
    }
  }
  return 1;
}

// take a -128..127 value and stbi__clamp it and convert to 0..255
stbi_inline static stbi_uc stbi__clamp(int x) {
  // trick to use a single test to catch both cases
  if ((unsigned int)x > 255) {
    if (x < 0)
      return 0;
    if (x > 255)
      return 255;
  }
  return (stbi_uc)x;
}

#define stbi__f2f(x) ((int)(((x)*4096 + 0.5)))
#define stbi__fsh(x) ((x) << 12)

// derived from jidctint -- DCT_ISLOW
#define STBI__IDCT_1D(s0, s1, s2, s3, s4, s5, s6, s7)                          \
  int t0, t1, t2, t3, p1, p2, p3, p4, p5, x0, x1, x2, x3;                      \
  p2 = s2;                                                                     \
  p3 = s6;                                                                     \
  p1 = (p2 + p3) * stbi__f2f(0.5411961f);                                      \
  t2 = p1 + p3 * stbi__f2f(-1.847759065f);                                     \
  t3 = p1 + p2 * stbi__f2f(0.765366865f);                                      \
  p2 = s0;                                                                     \
  p3 = s4;                                                                     \
  t0 = stbi__fsh(p2 + p3);                                                     \
  t1 = stbi__fsh(p2 - p3);                                                     \
  x0 = t0 + t3;                                                                \
  x3 = t0 - t3;                                                                \
  x1 = t1 + t2;                                                                \
  x2 = t1 - t2;                                                                \
  t0 = s7;                                                                     \
  t1 = s5;                                                                     \
  t2 = s3;                                                                     \
  t3 = s1;                                                                     \
  p3 = t0 + t2;                                                                \
  p4 = t1 + t3;                                                                \
  p1 = t0 + t3;                                                                \
  p2 = t1 + t2;                                                                \
  p5 = (p3 + p4) * stbi__f2f(1.175875602f);                                    \
  t0 = t0 * stbi__f2f(0.298631336f);                                           \
  t1 = t1 * stbi__f2f(2.053119869f);                                           \
  t2 = t2 * stbi__f2f(3.072711026f);                                           \
  t3 = t3 * stbi__f2f(1.501321110f);                                           \
  p1 = p5 + p1 * stbi__f2f(-0.899976223f);                                     \
  p2 = p5 + p2 * stbi__f2f(-2.562915447f);                                     \
  p3 = p3 * stbi__f2f(-1.961570560f);                                          \
  p4 = p4 * stbi__f2f(-0.390180644f);                                          \
  t3 += p1 + p4;                                                               \
  t2 += p2 + p3;                                                               \
  t1 += p2 + p4;                                                               \
  t0 += p1 + p3;

static void stbi__idct_block(stbi_uc *out, int out_stride, short data[64]) {
  int i, val[64], *v = val;
  stbi_uc *o;
  short *d = data;

  // columns
  for (i = 0; i < 8; ++i, ++d, ++v) {
    // if all zeroes, shortcut -- this avoids dequantizing 0s and IDCTing
    if (d[8] == 0 && d[16] == 0 && d[24] == 0 && d[32] == 0 && d[40] == 0 &&
        d[48] == 0 && d[56] == 0) {
      //    no shortcut                 0     seconds
      //    (1|2|3|4|5|6|7)==0          0     seconds
      //    all separate               -0.047 seconds
      //    1 && 2|3 && 4|5 && 6|7:    -0.047 seconds
      int dcterm = d[0] << 2;
      v[0] = v[8] = v[16] = v[24] = v[32] = v[40] = v[48] = v[56] = dcterm;
    } else {
      STBI__IDCT_1D(d[0], d[8], d[16], d[24], d[32], d[40], d[48], d[56])
      // constants scaled things up by 1<<12; let's bring them back
      // down, but keep 2 extra bits of precision
      x0 += 512;
      x1 += 512;
      x2 += 512;
      x3 += 512;
      v[0] = (x0 + t3) >> 10;
      v[56] = (x0 - t3) >> 10;
      v[8] = (x1 + t2) >> 10;
      v[48] = (x1 - t2) >> 10;
      v[16] = (x2 + t1) >> 10;
      v[40] = (x2 - t1) >> 10;
      v[24] = (x3 + t0) >> 10;
      v[32] = (x3 - t0) >> 10;
    }
  }

  for (i = 0, v = val, o = out; i < 8; ++i, v += 8, o += out_stride) {
    // no fast case since the first 1D IDCT spread components out
    STBI__IDCT_1D(v[0], v[1], v[2], v[3], v[4], v[5], v[6], v[7])
    // constants scaled things up by 1<<12, plus we had 1<<2 from first
    // loop, plus horizontal and vertical each scale by sqrt(8) so together
    // we've got an extra 1<<3, so 1<<17 total we need to remove.
    // so we want to round that, which means adding 0.5 * 1<<17,
    // aka 65536. Also, we'll end up with -128 to 127 that we want
    // to encode as 0..255 by adding 128, so we'll add that before the shift
    x0 += 65536 + (128 << 17);
    x1 += 65536 + (128 << 17);
    x2 += 65536 + (128 << 17);
    x3 += 65536 + (128 << 17);
    // tried computing the shifts into temps, or'ing the temps to see
    // if any were out of range, but that was slower
    o[0] = stbi__clamp((x0 + t3) >> 17);
    o[7] = stbi__clamp((x0 - t3) >> 17);
    o[1] = stbi__clamp((x1 + t2) >> 17);
    o[6] = stbi__clamp((x1 - t2) >> 17);
    o[2] = stbi__clamp((x2 + t1) >> 17);
    o[5] = stbi__clamp((x2 - t1) >> 17);
    o[3] = stbi__clamp((x3 + t0) >> 17);
    o[4] = stbi__clamp((x3 - t0) >> 17);
  }
}

#ifdef STBI_SSE2
// sse2 integer IDCT. not the fastest possible implementation but it
// produces bit-identical results to the generic C version so it's
// fully "transparent".
static void stbi__idct_simd(stbi_uc *out, int out_stride, short data[64]) {
  // This is constructed to match our regular (generic) integer IDCT exactly.
  __m128i row0, row1, row2, row3, row4, row5, row6, row7;
  __m128i tmp;

// dot product constant: even elems=x, odd elems=y
#define dct_const(x, y) _mm_setr_epi16((x), (y), (x), (y), (x), (y), (x), (y))

// out(0) = c0[even]*x + c0[odd]*y   (c0, x, y 16-bit, out 32-bit)
// out(1) = c1[even]*x + c1[odd]*y
#define dct_rot(out0, out1, x, y, c0, c1)                                      \
  __m128i c0##lo = _mm_unpacklo_epi16((x), (y));                               \
  __m128i c0##hi = _mm_unpackhi_epi16((x), (y));                               \
  __m128i out0##_l = _mm_madd_epi16(c0##lo, c0);                               \
  __m128i out0##_h = _mm_madd_epi16(c0##hi, c0);                               \
  __m128i out1##_l = _mm_madd_epi16(c0##lo, c1);                               \
  __m128i out1##_h = _mm_madd_epi16(c0##hi, c1)

// out = in << 12  (in 16-bit, out 32-bit)
#define dct_widen(out, in)                                                     \
  __m128i out##_l =                                                            \
      _mm_srai_epi32(_mm_unpacklo_epi16(_mm_setzero_si128(), (in)), 4);        \
  __m128i out##_h =                                                            \
      _mm_srai_epi32(_mm_unpackhi_epi16(_mm_setzero_si128(), (in)), 4)

// wide add
#define dct_wadd(out, a, b)                                                    \
  __m128i out##_l = _mm_add_epi32(a##_l, b##_l);                               \
  __m128i out##_h = _mm_add_epi32(a##_h, b##_h)

// wide sub
#define dct_wsub(out, a, b)                                                    \
  __m128i out##_l = _mm_sub_epi32(a##_l, b##_l);                               \
  __m128i out##_h = _mm_sub_epi32(a##_h, b##_h)

// butterfly a/b, add bias, then shift by "s" and pack
#define dct_bfly32o(out0, out1, a, b, bias, s)                                 \
  {                                                                            \
    __m128i abiased_l = _mm_add_epi32(a##_l, bias);                            \
    __m128i abiased_h = _mm_add_epi32(a##_h, bias);                            \
    dct_wadd(sum, abiased, b);                                                 \
    dct_wsub(dif, abiased, b);                                                 \
    out0 =                                                                     \
        _mm_packs_epi32(_mm_srai_epi32(sum_l, s), _mm_srai_epi32(sum_h, s));   \
    out1 =                                                                     \
        _mm_packs_epi32(_mm_srai_epi32(dif_l, s), _mm_srai_epi32(dif_h, s));   \
  }

// 8-bit interleave step (for transposes)
#define dct_interleave8(a, b)                                                  \
  tmp = a;                                                                     \
  a = _mm_unpacklo_epi8(a, b);                                                 \
  b = _mm_unpackhi_epi8(tmp, b)

// 16-bit interleave step (for transposes)
#define dct_interleave16(a, b)                                                 \
  tmp = a;                                                                     \
  a = _mm_unpacklo_epi16(a, b);                                                \
  b = _mm_unpackhi_epi16(tmp, b)

#define dct_pass(bias, shift)                                                  \
  {                                                                            \
    /* even part */                                                            \
    dct_rot(t2e, t3e, row2, row6, rot0_0, rot0_1);                             \
    __m128i sum04 = _mm_add_epi16(row0, row4);                                 \
    __m128i dif04 = _mm_sub_epi16(row0, row4);                                 \
    dct_widen(t0e, sum04);                                                     \
    dct_widen(t1e, dif04);                                                     \
    dct_wadd(x0, t0e, t3e);                                                    \
    dct_wsub(x3, t0e, t3e);                                                    \
    dct_wadd(x1, t1e, t2e);                                                    \
    dct_wsub(x2, t1e, t2e);                                                    \
    /* odd part */                                                             \
    dct_rot(y0o, y2o, row7, row3, rot2_0, rot2_1);                             \
    dct_rot(y1o, y3o, row5, row1, rot3_0, rot3_1);                             \
    __m128i sum17 = _mm_add_epi16(row1, row7);                                 \
    __m128i sum35 = _mm_add_epi16(row3, row5);                                 \
    dct_rot(y4o, y5o, sum17, sum35, rot1_0, rot1_1);                           \
    dct_wadd(x4, y0o, y4o);                                                    \
    dct_wadd(x5, y1o, y5o);                                                    \
    dct_wadd(x6, y2o, y5o);                                                    \
    dct_wadd(x7, y3o, y4o);                                                    \
    dct_bfly32o(row0, row7, x0, x7, bias, shift);                              \
    dct_bfly32o(row1, row6, x1, x6, bias, shift);                              \
    dct_bfly32o(row2, row5, x2, x5, bias, shift);                              \
    dct_bfly32o(row3, row4, x3, x4, bias, shift);                              \
  }

  __m128i rot0_0 = dct_const(stbi__f2f(0.5411961f),
                             stbi__f2f(0.5411961f) + stbi__f2f(-1.847759065f));
  __m128i rot0_1 = dct_const(stbi__f2f(0.5411961f) + stbi__f2f(0.765366865f),
                             stbi__f2f(0.5411961f));
  __m128i rot1_0 = dct_const(stbi__f2f(1.175875602f) + stbi__f2f(-0.899976223f),
                             stbi__f2f(1.175875602f));
  __m128i rot1_1 =
      dct_const(stbi__f2f(1.175875602f),
                stbi__f2f(1.175875602f) + stbi__f2f(-2.562915447f));
  __m128i rot2_0 = dct_const(stbi__f2f(-1.961570560f) + stbi__f2f(0.298631336f),
                             stbi__f2f(-1.961570560f));
  __m128i rot2_1 =
      dct_const(stbi__f2f(-1.961570560f),
                stbi__f2f(-1.961570560f) + stbi__f2f(3.072711026f));
  __m128i rot3_0 = dct_const(stbi__f2f(-0.390180644f) + stbi__f2f(2.053119869f),
                             stbi__f2f(-0.390180644f));
  __m128i rot3_1 =
      dct_const(stbi__f2f(-0.390180644f),
                stbi__f2f(-0.390180644f) + stbi__f2f(1.501321110f));

  // rounding biases in column/row passes, see stbi__idct_block for explanation.
  __m128i bias_0 = _mm_set1_epi32(512);
  __m128i bias_1 = _mm_set1_epi32(65536 + (128 << 17));

  // load
  row0 = _mm_load_si128((const __m128i *)(data + 0 * 8));
  row1 = _mm_load_si128((const __m128i *)(data + 1 * 8));
  row2 = _mm_load_si128((const __m128i *)(data + 2 * 8));
  row3 = _mm_load_si128((const __m128i *)(data + 3 * 8));
  row4 = _mm_load_si128((const __m128i *)(data + 4 * 8));
  row5 = _mm_load_si128((const __m128i *)(data + 5 * 8));
  row6 = _mm_load_si128((const __m128i *)(data + 6 * 8));
  row7 = _mm_load_si128((const __m128i *)(data + 7 * 8));

  // column pass
  dct_pass(bias_0, 10);

  {
    // 16bit 8x8 transpose pass 1
    dct_interleave16(row0, row4);
    dct_interleave16(row1, row5);
    dct_interleave16(row2, row6);
    dct_interleave16(row3, row7);

    // transpose pass 2
    dct_interleave16(row0, row2);
    dct_interleave16(row1, row3);
    dct_interleave16(row4, row6);
    dct_interleave16(row5, row7);

    // transpose pass 3
    dct_interleave16(row0, row1);
    dct_interleave16(row2, row3);
    dct_interleave16(row4, row5);
    dct_interleave16(row6, row7);
  }

  // row pass
  dct_pass(bias_1, 17);

  {
    // pack
    __m128i p0 = _mm_packus_epi16(row0, row1); // a0a1a2a3...a7b0b1b2b3...b7
    __m128i p1 = _mm_packus_epi16(row2, row3);
    __m128i p2 = _mm_packus_epi16(row4, row5);
    __m128i p3 = _mm_packus_epi16(row6, row7);

    // 8bit 8x8 transpose pass 1
    dct_interleave8(p0, p2); // a0e0a1e1...
    dct_interleave8(p1, p3); // c0g0c1g1...

    // transpose pass 2
    dct_interleave8(p0, p1); // a0c0e0g0...
    dct_interleave8(p2, p3); // b0d0f0h0...

    // transpose pass 3
    dct_interleave8(p0, p2); // a0b0c0d0...
    dct_interleave8(p1, p3); // a4b4c4d4...

    // store
    _mm_storel_epi64((__m128i *)out, p0);
    out += out_stride;
    _mm_storel_epi64((__m128i *)out, _mm_shuffle_epi32(p0, 0x4e));
    out += out_stride;
    _mm_storel_epi64((__m128i *)out, p2);
    out += out_stride;
    _mm_storel_epi64((__m128i *)out, _mm_shuffle_epi32(p2, 0x4e));
    out += out_stride;
    _mm_storel_epi64((__m128i *)out, p1);
    out += out_stride;
    _mm_storel_epi64((__m128i *)out, _mm_shuffle_epi32(p1, 0x4e));
    out += out_stride;
    _mm_storel_epi64((__m128i *)out, p3);
    out += out_stride;
    _mm_storel_epi64((__m128i *)out, _mm_shuffle_epi32(p3, 0x4e));
  }

#undef dct_const
#undef dct_rot
#undef dct_widen
#undef dct_wadd
#undef dct_wsub
#undef dct_bfly32o
#undef dct_interleave8
#undef dct_interleave16
#undef dct_pass
}

#endif // STBI_SSE2

#ifdef STBI_NEON

// NEON integer IDCT. should produce bit-identical
// results to the generic C version.
static void stbi__idct_simd(stbi_uc *out, int out_stride, short data[64]) {
  int16x8_t row0, row1, row2, row3, row4, row5, row6, row7;

  int16x4_t rot0_0 = vdup_n_s16(stbi__f2f(0.5411961f));
  int16x4_t rot0_1 = vdup_n_s16(stbi__f2f(-1.847759065f));
  int16x4_t rot0_2 = vdup_n_s16(stbi__f2f(0.765366865f));
  int16x4_t rot1_0 = vdup_n_s16(stbi__f2f(1.175875602f));
  int16x4_t rot1_1 = vdup_n_s16(stbi__f2f(-0.899976223f));
  int16x4_t rot1_2 = vdup_n_s16(stbi__f2f(-2.562915447f));
  int16x4_t rot2_0 = vdup_n_s16(stbi__f2f(-1.961570560f));
  int16x4_t rot2_1 = vdup_n_s16(stbi__f2f(-0.390180644f));
  int16x4_t rot3_0 = vdup_n_s16(stbi__f2f(0.298631336f));
  int16x4_t rot3_1 = vdup_n_s16(stbi__f2f(2.053119869f));
  int16x4_t rot3_2 = vdup_n_s16(stbi__f2f(3.072711026f));
  int16x4_t rot3_3 = vdup_n_s16(stbi__f2f(1.501321110f));

#define dct_long_mul(out, inq, coeff)                                          \
  int32x4_t out##_l = vmull_s16(vget_low_s16(inq), coeff);                     \
  int32x4_t out##_h = vmull_s16(vget_high_s16(inq), coeff)

#define dct_long_mac(out, acc, inq, coeff)                                     \
  int32x4_t out##_l = vmlal_s16(acc##_l, vget_low_s16(inq), coeff);            \
  int32x4_t out##_h = vmlal_s16(acc##_h, vget_high_s16(inq), coeff)

#define dct_widen(out, inq)                                                    \
  int32x4_t out##_l = vshll_n_s16(vget_low_s16(inq), 12);                      \
  int32x4_t out##_h = vshll_n_s16(vget_high_s16(inq), 12)

// wide add
#define dct_wadd(out, a, b)                                                    \
  int32x4_t out##_l = vaddq_s32(a##_l, b##_l);                                 \
  int32x4_t out##_h = vaddq_s32(a##_h, b##_h)

// wide sub
#define dct_wsub(out, a, b)                                                    \
  int32x4_t out##_l = vsubq_s32(a##_l, b##_l);                                 \
  int32x4_t out##_h = vsubq_s32(a##_h, b##_h)

// butterfly a/b, then shift using "shiftop" by "s" and pack
#define dct_bfly32o(out0, out1, a, b, shiftop, s)                              \
  {                                                                            \
    dct_wadd(sum, a, b);                                                       \
    dct_wsub(dif, a, b);                                                       \
    out0 = vcombine_s16(shiftop(sum_l, s), shiftop(sum_h, s));                 \
    out1 = vcombine_s16(shiftop(dif_l, s), shiftop(dif_h, s));                 \
  }

#define dct_pass(shiftop, shift)                                               \
  {                                                                            \
    /* even part */                                                            \
    int16x8_t sum26 = vaddq_s16(row2, row6);                                   \
    dct_long_mul(p1e, sum26, rot0_0);                                          \
    dct_long_mac(t2e, p1e, row6, rot0_1);                                      \
    dct_long_mac(t3e, p1e, row2, rot0_2);                                      \
    int16x8_t sum04 = vaddq_s16(row0, row4);                                   \
    int16x8_t dif04 = vsubq_s16(row0, row4);                                   \
    dct_widen(t0e, sum04);                                                     \
    dct_widen(t1e, dif04);                                                     \
    dct_wadd(x0, t0e, t3e);                                                    \
    dct_wsub(x3, t0e, t3e);                                                    \
    dct_wadd(x1, t1e, t2e);                                                    \
    dct_wsub(x2, t1e, t2e);                                                    \
    /* odd part */                                                             \
    int16x8_t sum15 = vaddq_s16(row1, row5);                                   \
    int16x8_t sum17 = vaddq_s16(row1, row7);                                   \
    int16x8_t sum35 = vaddq_s16(row3, row5);                                   \
    int16x8_t sum37 = vaddq_s16(row3, row7);                                   \
    int16x8_t sumodd = vaddq_s16(sum17, sum35);                                \
    dct_long_mul(p5o, sumodd, rot1_0);                                         \
    dct_long_mac(p1o, p5o, sum17, rot1_1);                                     \
    dct_long_mac(p2o, p5o, sum35, rot1_2);                                     \
    dct_long_mul(p3o, sum37, rot2_0);                                          \
    dct_long_mul(p4o, sum15, rot2_1);                                          \
    dct_wadd(sump13o, p1o, p3o);                                               \
    dct_wadd(sump24o, p2o, p4o);                                               \
    dct_wadd(sump23o, p2o, p3o);                                               \
    dct_wadd(sump14o, p1o, p4o);                                               \
    dct_long_mac(x4, sump13o, row7, rot3_0);                                   \
    dct_long_mac(x5, sump24o, row5, rot3_1);                                   \
    dct_long_mac(x6, sump23o, row3, rot3_2);                                   \
    dct_long_mac(x7, sump14o, row1, rot3_3);                                   \
    dct_bfly32o(row0, row7, x0, x7, shiftop, shift);                           \
    dct_bfly32o(row1, row6, x1, x6, shiftop, shift);                           \
    dct_bfly32o(row2, row5, x2, x5, shiftop, shift);                           \
    dct_bfly32o(row3, row4, x3, x4, shiftop, shift);                           \
  }

  // load
  row0 = vld1q_s16(data + 0 * 8);
  row1 = vld1q_s16(data + 1 * 8);
  row2 = vld1q_s16(data + 2 * 8);
  row3 = vld1q_s16(data + 3 * 8);
  row4 = vld1q_s16(data + 4 * 8);
  row5 = vld1q_s16(data + 5 * 8);
  row6 = vld1q_s16(data + 6 * 8);
  row7 = vld1q_s16(data + 7 * 8);

  // add DC bias
  row0 = vaddq_s16(row0, vsetq_lane_s16(1024, vdupq_n_s16(0), 0));

  // column pass
  dct_pass(vrshrn_n_s32, 10);

  // 16bit 8x8 transpose
  {
// these three map to a single VTRN.16, VTRN.32, and VSWP, respectively.
// whether compilers actually get this is another story, sadly.
#define dct_trn16(x, y)                                                        \
  {                                                                            \
    int16x8x2_t t = vtrnq_s16(x, y);                                           \
    x = t.val[0];                                                              \
    y = t.val[1];                                                              \
  }
#define dct_trn32(x, y)                                                        \
  {                                                                            \
    int32x4x2_t t =                                                            \
        vtrnq_s32(vreinterpretq_s32_s16(x), vreinterpretq_s32_s16(y));         \
    x = vreinterpretq_s16_s32(t.val[0]);                                       \
    y = vreinterpretq_s16_s32(t.val[1]);                                       \
  }
#define dct_trn64(x, y)                                                        \
  {                                                                            \
    int16x8_t x0 = x;                                                          \
    int16x8_t y0 = y;                                                          \
    x = vcombine_s16(vget_low_s16(x0), vget_low_s16(y0));                      \
    y = vcombine_s16(vget_high_s16(x0), vget_high_s16(y0));                    \
  }

    // pass 1
    dct_trn16(row0, row1); // a0b0a2b2a4b4a6b6
    dct_trn16(row2, row3);
    dct_trn16(row4, row5);
    dct_trn16(row6, row7);

    // pass 2
    dct_trn32(row0, row2); // a0b0c0d0a4b4c4d4
    dct_trn32(row1, row3);
    dct_trn32(row4, row6);
    dct_trn32(row5, row7);

    // pass 3
    dct_trn64(row0, row4); // a0b0c0d0e0f0g0h0
    dct_trn64(row1, row5);
    dct_trn64(row2, row6);
    dct_trn64(row3, row7);

#undef dct_trn16
#undef dct_trn32
#undef dct_trn64
  }

  // row pass
  // vrshrn_n_s32 only supports shifts up to 16, we need
  // 17. so do a non-rounding shift of 16 first then follow
  // up with a rounding shift by 1.
  dct_pass(vshrn_n_s32, 16);

  {
    // pack and round
    uint8x8_t p0 = vqrshrun_n_s16(row0, 1);
    uint8x8_t p1 = vqrshrun_n_s16(row1, 1);
    uint8x8_t p2 = vqrshrun_n_s16(row2, 1);
    uint8x8_t p3 = vqrshrun_n_s16(row3, 1);
    uint8x8_t p4 = vqrshrun_n_s16(row4, 1);
    uint8x8_t p5 = vqrshrun_n_s16(row5, 1);
    uint8x8_t p6 = vqrshrun_n_s16(row6, 1);
    uint8x8_t p7 = vqrshrun_n_s16(row7, 1);

    // again, these can translate into one instruction, but often don't.
#define dct_trn8_8(x, y)                                                       \
  {                                                                            \
    uint8x8x2_t t = vtrn_u8(x, y);                                             \
    x = t.val[0];                                                              \
    y = t.val[1];                                                              \
  }
#define dct_trn8_16(x, y)                                                      \
  {                                                                            \
    uint16x4x2_t t = vtrn_u16(vreinterpret_u16_u8(x), vreinterpret_u16_u8(y)); \
    x = vreinterpret_u8_u16(t.val[0]);                                         \
    y = vreinterpret_u8_u16(t.val[1]);                                         \
  }
#define dct_trn8_32(x, y)                                                      \
  {                                                                            \
    uint32x2x2_t t = vtrn_u32(vreinterpret_u32_u8(x), vreinterpret_u32_u8(y)); \
    x = vreinterpret_u8_u32(t.val[0]);                                         \
    y = vreinterpret_u8_u32(t.val[1]);                                         \
  }

    // sadly can't use interleaved stores here since we only write
    // 8 bytes to each scan line!

    // 8x8 8-bit transpose pass 1
    dct_trn8_8(p0, p1);
    dct_trn8_8(p2, p3);
    dct_trn8_8(p4, p5);
    dct_trn8_8(p6, p7);

    // pass 2
    dct_trn8_16(p0, p2);
    dct_trn8_16(p1, p3);
    dct_trn8_16(p4, p6);
    dct_trn8_16(p5, p7);

    // pass 3
    dct_trn8_32(p0, p4);
    dct_trn8_32(p1, p5);
    dct_trn8_32(p2, p6);
    dct_trn8_32(p3, p7);

    // store
    vst1_u8(out, p0);
    out += out_stride;
    vst1_u8(out, p1);
    out += out_stride;
    vst1_u8(out, p2);
    out += out_stride;
    vst1_u8(out, p3);
    out += out_stride;
    vst1_u8(out, p4);
    out += out_stride;
    vst1_u8(out, p5);
    out += out_stride;
    vst1_u8(out, p6);
    out += out_stride;
    vst1_u8(out, p7);

#undef dct_trn8_8
#undef dct_trn8_16
#undef dct_trn8_32
  }

#undef dct_long_mul
#undef dct_long_mac
#undef dct_widen
#undef dct_wadd
#undef dct_wsub
#undef dct_bfly32o
#undef dct_pass
}

#endif // STBI_NEON

#define STBI__MARKER_none 0xff
// if there's a pending marker from the entropy stream, return that
// otherwise, fetch from the stream and get a marker. if there's no
// marker, return 0xff, which is never a valid marker value
static stbi_uc stbi__get_marker(stbi__jpeg *j) {
  stbi_uc x;
  if (j->marker != STBI__MARKER_none) {
    x = j->marker;
    j->marker = STBI__MARKER_none;
    return x;
  }
  x = stbi__get8(j->s);
  if (x != 0xff)
    return STBI__MARKER_none;
  while (x == 0xff)
    x = stbi__get8(j->s); // consume repeated 0xff fill bytes
  return x;
}

// in each scan, we'll have scan_n components, and the order
// of the components is specified by order[]
#define STBI__RESTART(x) ((x) >= 0xd0 && (x) <= 0xd7)

// after a restart interval, stbi__jpeg_reset the entropy decoder and
// the dc prediction
static void stbi__jpeg_reset(stbi__jpeg *j) {
  j->code_bits = 0;
  j->code_buffer = 0;
  j->nomore = 0;
  j->img_comp[0].dc_pred = j->img_comp[1].dc_pred = j->img_comp[2].dc_pred =
      j->img_comp[3].dc_pred = 0;
  j->marker = STBI__MARKER_none;
  j->todo = j->restart_interval ? j->restart_interval : 0x7fffffff;
  j->eob_run = 0;
  // no more than 1<<31 MCUs if no restart_interal? that's plenty safe,
  // since we don't even allow 1<<30 pixels
}

static int stbi__parse_entropy_coded_data(stbi__jpeg *z) {
  stbi__jpeg_reset(z);
  if (!z->progressive) {
    if (z->scan_n == 1) {
      int i, j;
      STBI_SIMD_ALIGN(short, data[64]);
      int n = z->order[0];
      // non-interleaved data, we just need to process one block at a time,
      // in trivial scanline order
      // number of blocks to do just depends on how many actual "pixels" this
      // component has, independent of interleaved MCU blocking and such
      int w = (z->img_comp[n].x + 7) >> 3;
      int h = (z->img_comp[n].y + 7) >> 3;
      for (j = 0; j < h; ++j) {
        for (i = 0; i < w; ++i) {
          int ha = z->img_comp[n].ha;
          if (!stbi__jpeg_decode_block(z, data, z->huff_dc + z->img_comp[n].hd,
                                       z->huff_ac + ha, z->fast_ac[ha], n,
                                       z->dequant[z->img_comp[n].tq]))
            return 0;
          z->idct_block_kernel(z->img_comp[n].data + z->img_comp[n].w2 * j * 8 +
                                   i * 8,
                               z->img_comp[n].w2, data);
          // every data block is an MCU, so countdown the restart interval
          if (--z->todo <= 0) {
            if (z->code_bits < 24)
              stbi__grow_buffer_unsafe(z);
            // if it's NOT a restart, then just bail, so we get corrupt data
            // rather than no data
            if (!STBI__RESTART(z->marker))
              return 1;
            stbi__jpeg_reset(z);
          }
        }
      }
      return 1;
    } else { // interleaved
      int i, j, k, x, y;
      STBI_SIMD_ALIGN(short, data[64]);
      for (j = 0; j < z->img_mcu_y; ++j) {
        for (i = 0; i < z->img_mcu_x; ++i) {
          // scan an interleaved mcu... process scan_n components in order
          for (k = 0; k < z->scan_n; ++k) {
            int n = z->order[k];
            // scan out an mcu's worth of this component; that's just determined
            // by the basic H and V specified for the component
            for (y = 0; y < z->img_comp[n].v; ++y) {
              for (x = 0; x < z->img_comp[n].h; ++x) {
                int x2 = (i * z->img_comp[n].h + x) * 8;
                int y2 = (j * z->img_comp[n].v + y) * 8;
                int ha = z->img_comp[n].ha;
                if (!stbi__jpeg_decode_block(z, data,
                                             z->huff_dc + z->img_comp[n].hd,
                                             z->huff_ac + ha, z->fast_ac[ha], n,
                                             z->dequant[z->img_comp[n].tq]))
                  return 0;
                z->idct_block_kernel(z->img_comp[n].data +
                                         z->img_comp[n].w2 * y2 + x2,
                                     z->img_comp[n].w2, data);
              }
            }
          }
          // after all interleaved components, that's an interleaved MCU,
          // so now count down the restart interval
          if (--z->todo <= 0) {
            if (z->code_bits < 24)
              stbi__grow_buffer_unsafe(z);
            if (!STBI__RESTART(z->marker))
              return 1;
            stbi__jpeg_reset(z);
          }
        }
      }
      return 1;
    }
  } else {
    if (z->scan_n == 1) {
      int i, j;
      int n = z->order[0];
      // non-interleaved data, we just need to process one block at a time,
      // in trivial scanline order
      // number of blocks to do just depends on how many actual "pixels" this
      // component has, independent of interleaved MCU blocking and such
      int w = (z->img_comp[n].x + 7) >> 3;
      int h = (z->img_comp[n].y + 7) >> 3;
      for (j = 0; j < h; ++j) {
        for (i = 0; i < w; ++i) {
          short *data =
              z->img_comp[n].coeff + 64 * (i + j * z->img_comp[n].coeff_w);
          if (z->spec_start == 0) {
            if (!stbi__jpeg_decode_block_prog_dc(
                    z, data, &z->huff_dc[z->img_comp[n].hd], n))
              return 0;
          } else {
            int ha = z->img_comp[n].ha;
            if (!stbi__jpeg_decode_block_prog_ac(z, data, &z->huff_ac[ha],
                                                 z->fast_ac[ha]))
              return 0;
          }
          // every data block is an MCU, so countdown the restart interval
          if (--z->todo <= 0) {
            if (z->code_bits < 24)
              stbi__grow_buffer_unsafe(z);
            if (!STBI__RESTART(z->marker))
              return 1;
            stbi__jpeg_reset(z);
          }
        }
      }
      return 1;
    } else { // interleaved
      int i, j, k, x, y;
      for (j = 0; j < z->img_mcu_y; ++j) {
        for (i = 0; i < z->img_mcu_x; ++i) {
          // scan an interleaved mcu... process scan_n components in order
          for (k = 0; k < z->scan_n; ++k) {
            int n = z->order[k];
            // scan out an mcu's worth of this component; that's just determined
            // by the basic H and V specified for the component
            for (y = 0; y < z->img_comp[n].v; ++y) {
              for (x = 0; x < z->img_comp[n].h; ++x) {
                int x2 = (i * z->img_comp[n].h + x);
                int y2 = (j * z->img_comp[n].v + y);
                short *data = z->img_comp[n].coeff +
                              64 * (x2 + y2 * z->img_comp[n].coeff_w);
                if (!stbi__jpeg_decode_block_prog_dc(
                        z, data, &z->huff_dc[z->img_comp[n].hd], n))
                  return 0;
              }
            }
          }
          // after all interleaved components, that's an interleaved MCU,
          // so now count down the restart interval
          if (--z->todo <= 0) {
            if (z->code_bits < 24)
              stbi__grow_buffer_unsafe(z);
            if (!STBI__RESTART(z->marker))
              return 1;
            stbi__jpeg_reset(z);
          }
        }
      }
      return 1;
    }
  }
}

static void stbi__jpeg_dequantize(short *data, stbi__uint16 *dequant) {
  int i;
  for (i = 0; i < 64; ++i)
    data[i] *= dequant[i];
}

static void stbi__jpeg_finish(stbi__jpeg *z) {
  if (z->progressive) {
    // dequantize and idct the data
    int i, j, n;
    for (n = 0; n < z->s->img_n; ++n) {
      int w = (z->img_comp[n].x + 7) >> 3;
      int h = (z->img_comp[n].y + 7) >> 3;
      for (j = 0; j < h; ++j) {
        for (i = 0; i < w; ++i) {
          short *data =
              z->img_comp[n].coeff + 64 * (i + j * z->img_comp[n].coeff_w);
          stbi__jpeg_dequantize(data, z->dequant[z->img_comp[n].tq]);
          z->idct_block_kernel(z->img_comp[n].data + z->img_comp[n].w2 * j * 8 +
                                   i * 8,
                               z->img_comp[n].w2, data);
        }
      }
    }
  }
}

static int stbi__process_marker(stbi__jpeg *z, int m) {
  int L;
  switch (m) {
  case STBI__MARKER_none: // no marker found
    return stbi__err("expected marker", "Corrupt JPEG");

  case 0xDD: // DRI - specify restart interval
    if (stbi__get16be(z->s) != 4)
      return stbi__err("bad DRI len", "Corrupt JPEG");
    z->restart_interval = stbi__get16be(z->s);
    return 1;

  case 0xDB: // DQT - define quantization table
    L = stbi__get16be(z->s) - 2;
    while (L > 0) {
      int q = stbi__get8(z->s);
      int p = q >> 4, sixteen = (p != 0);
      int t = q & 15, i;
      if (p != 0 && p != 1)
        return stbi__err("bad DQT type", "Corrupt JPEG");
      if (t > 3)
        return stbi__err("bad DQT table", "Corrupt JPEG");

      for (i = 0; i < 64; ++i)
        z->dequant[t][stbi__jpeg_dezigzag[i]] =
            (stbi__uint16)(sixteen ? stbi__get16be(z->s) : stbi__get8(z->s));
      L -= (sixteen ? 129 : 65);
    }
    return L == 0;

  case 0xC4: // DHT - define huffman table
    L = stbi__get16be(z->s) - 2;
    while (L > 0) {
      stbi_uc *v;
      int sizes[16], i, n = 0;
      int q = stbi__get8(z->s);
      int tc = q >> 4;
      int th = q & 15;
      if (tc > 1 || th > 3)
        return stbi__err("bad DHT header", "Corrupt JPEG");
      for (i = 0; i < 16; ++i) {
        sizes[i] = stbi__get8(z->s);
        n += sizes[i];
      }
      L -= 17;
      if (tc == 0) {
        if (!stbi__build_huffman(z->huff_dc + th, sizes))
          return 0;
        v = z->huff_dc[th].values;
      } else {
        if (!stbi__build_huffman(z->huff_ac + th, sizes))
          return 0;
        v = z->huff_ac[th].values;
      }
      for (i = 0; i < n; ++i)
        v[i] = stbi__get8(z->s);
      if (tc != 0)
        stbi__build_fast_ac(z->fast_ac[th], z->huff_ac + th);
      L -= n;
    }
    return L == 0;
  }

  // check for comment block or APP blocks
  if ((m >= 0xE0 && m <= 0xEF) || m == 0xFE) {
    L = stbi__get16be(z->s);
    if (L < 2) {
      if (m == 0xFE)
        return stbi__err("bad COM len", "Corrupt JPEG");
      else
        return stbi__err("bad APP len", "Corrupt JPEG");
    }
    L -= 2;

    if (m == 0xE0 && L >= 5) { // JFIF APP0 segment
      static const unsigned char tag[5] = {'J', 'F', 'I', 'F', '\0'};
      int ok = 1;
      int i;
      for (i = 0; i < 5; ++i)
        if (stbi__get8(z->s) != tag[i])
          ok = 0;
      L -= 5;
      if (ok)
        z->jfif = 1;
    } else if (m == 0xEE && L >= 12) { // Adobe APP14 segment
      static const unsigned char tag[6] = {'A', 'd', 'o', 'b', 'e', '\0'};
      int ok = 1;
      int i;
      for (i = 0; i < 6; ++i)
        if (stbi__get8(z->s) != tag[i])
          ok = 0;
      L -= 6;
      if (ok) {
        stbi__get8(z->s);                            // version
        stbi__get16be(z->s);                         // flags0
        stbi__get16be(z->s);                         // flags1
        z->app14_color_transform = stbi__get8(z->s); // color transform
        L -= 6;
      }
    }

    stbi__skip(z->s, L);
    return 1;
  }

  return stbi__err("unknown marker", "Corrupt JPEG");
}

// after we see SOS
static int stbi__process_scan_header(stbi__jpeg *z) {
  int i;
  int Ls = stbi__get16be(z->s);
  z->scan_n = stbi__get8(z->s);
  if (z->scan_n < 1 || z->scan_n > 4 || z->scan_n > (int)z->s->img_n)
    return stbi__err("bad SOS component count", "Corrupt JPEG");
  if (Ls != 6 + 2 * z->scan_n)
    return stbi__err("bad SOS len", "Corrupt JPEG");
  for (i = 0; i < z->scan_n; ++i) {
    int id = stbi__get8(z->s), which;
    int q = stbi__get8(z->s);
    for (which = 0; which < z->s->img_n; ++which)
      if (z->img_comp[which].id == id)
        break;
    if (which == z->s->img_n)
      return 0; // no match
    z->img_comp[which].hd = q >> 4;
    if (z->img_comp[which].hd > 3)
      return stbi__err("bad DC huff", "Corrupt JPEG");
    z->img_comp[which].ha = q & 15;
    if (z->img_comp[which].ha > 3)
      return stbi__err("bad AC huff", "Corrupt JPEG");
    z->order[i] = which;
  }

  {
    int aa;
    z->spec_start = stbi__get8(z->s);
    z->spec_end = stbi__get8(z->s); // should be 63, but might be 0
    aa = stbi__get8(z->s);
    z->succ_high = (aa >> 4);
    z->succ_low = (aa & 15);
    if (z->progressive) {
      if (z->spec_start > 63 || z->spec_end > 63 ||
          z->spec_start > z->spec_end || z->succ_high > 13 || z->succ_low > 13)
        return stbi__err("bad SOS", "Corrupt JPEG");
    } else {
      if (z->spec_start != 0)
        return stbi__err("bad SOS", "Corrupt JPEG");
      if (z->succ_high != 0 || z->succ_low != 0)
        return stbi__err("bad SOS", "Corrupt JPEG");
      z->spec_end = 63;
    }
  }

  return 1;
}

static int stbi__free_jpeg_components(stbi__jpeg *z, int ncomp, int why) {
  int i;
  for (i = 0; i < ncomp; ++i) {
    if (z->img_comp[i].raw_data) {
      STBI_FREE(z->img_comp[i].raw_data);
      z->img_comp[i].raw_data = NULL;
      z->img_comp[i].data = NULL;
    }
    if (z->img_comp[i].raw_coeff) {
      STBI_FREE(z->img_comp[i].raw_coeff);
      z->img_comp[i].raw_coeff = 0;
      z->img_comp[i].coeff = 0;
    }
    if (z->img_comp[i].linebuf) {
      STBI_FREE(z->img_comp[i].linebuf);
      z->img_comp[i].linebuf = NULL;
    }
  }
  return why;
}

static int stbi__process_frame_header(stbi__jpeg *z, int scan) {
  stbi__context *s = z->s;
  int Lf, p, i, q, h_max = 1, v_max = 1, c;
  Lf = stbi__get16be(s);
  if (Lf < 11)
    return stbi__err("bad SOF len", "Corrupt JPEG"); // JPEG
  p = stbi__get8(s);
  if (p != 8)
    return stbi__err("only 8-bit",
                     "JPEG format not supported: 8-bit only"); // JPEG baseline
  s->img_y = stbi__get16be(s);
  if (s->img_y == 0)
    return stbi__err("no header height",
                     "JPEG format not supported: delayed height"); // Legal, but
                                                                   // we don't
                                                                   // handle
                                                                   // it--but
                                                                   // neither
                                                                   // does IJG
  s->img_x = stbi__get16be(s);
  if (s->img_x == 0)
    return stbi__err("0 width", "Corrupt JPEG"); // JPEG requires
  c = stbi__get8(s);
  if (c != 3 && c != 1 && c != 4)
    return stbi__err("bad component count", "Corrupt JPEG");
  s->img_n = c;
  for (i = 0; i < c; ++i) {
    z->img_comp[i].data = NULL;
    z->img_comp[i].linebuf = NULL;
  }

  if (Lf != 8 + 3 * s->img_n)
    return stbi__err("bad SOF len", "Corrupt JPEG");

  z->rgb = 0;
  for (i = 0; i < s->img_n; ++i) {
    static unsigned char rgb[3] = {'R', 'G', 'B'};
    z->img_comp[i].id = stbi__get8(s);
    if (s->img_n == 3 && z->img_comp[i].id == rgb[i])
      ++z->rgb;
    q = stbi__get8(s);
    z->img_comp[i].h = (q >> 4);
    if (!z->img_comp[i].h || z->img_comp[i].h > 4)
      return stbi__err("bad H", "Corrupt JPEG");
    z->img_comp[i].v = q & 15;
    if (!z->img_comp[i].v || z->img_comp[i].v > 4)
      return stbi__err("bad V", "Corrupt JPEG");
    z->img_comp[i].tq = stbi__get8(s);
    if (z->img_comp[i].tq > 3)
      return stbi__err("bad TQ", "Corrupt JPEG");
  }

  if (scan != STBI__SCAN_load)
    return 1;

  if (!stbi__mad3sizes_valid(s->img_x, s->img_y, s->img_n, 0))
    return stbi__err("too large", "Image too large to decode");

  for (i = 0; i < s->img_n; ++i) {
    if (z->img_comp[i].h > h_max)
      h_max = z->img_comp[i].h;
    if (z->img_comp[i].v > v_max)
      v_max = z->img_comp[i].v;
  }

  // compute interleaved mcu info
  z->img_h_max = h_max;
  z->img_v_max = v_max;
  z->img_mcu_w = h_max * 8;
  z->img_mcu_h = v_max * 8;
  // these sizes can't be more than 17 bits
  z->img_mcu_x = (s->img_x + z->img_mcu_w - 1) / z->img_mcu_w;
  z->img_mcu_y = (s->img_y + z->img_mcu_h - 1) / z->img_mcu_h;

  for (i = 0; i < s->img_n; ++i) {
    // number of effective pixels (e.g. for non-interleaved MCU)
    z->img_comp[i].x = (s->img_x * z->img_comp[i].h + h_max - 1) / h_max;
    z->img_comp[i].y = (s->img_y * z->img_comp[i].v + v_max - 1) / v_max;
    // to simplify generation, we'll allocate enough memory to decode
    // the bogus oversized data from using interleaved MCUs and their
    // big blocks (e.g. a 16x16 iMCU on an image of width 33); we won't
    // discard the extra data until colorspace conversion
    //
    // img_mcu_x, img_mcu_y: <=17 bits; comp[i].h and .v are <=4 (checked
    // earlier) so these muls can't overflow with 32-bit ints (which we require)
    z->img_comp[i].w2 = z->img_mcu_x * z->img_comp[i].h * 8;
    z->img_comp[i].h2 = z->img_mcu_y * z->img_comp[i].v * 8;
    z->img_comp[i].coeff = 0;
    z->img_comp[i].raw_coeff = 0;
    z->img_comp[i].linebuf = NULL;
    z->img_comp[i].raw_data =
        stbi__malloc_mad2(z->img_comp[i].w2, z->img_comp[i].h2, 15);
    if (z->img_comp[i].raw_data == NULL)
      return stbi__free_jpeg_components(z, i + 1,
                                        stbi__err("outofmem", "Out of memory"));
    // align blocks for idct using mmx/sse
    z->img_comp[i].data =
        (stbi_uc *)(((size_t)z->img_comp[i].raw_data + 15) & ~15);
    if (z->progressive) {
      // w2, h2 are multiples of 8 (see above)
      z->img_comp[i].coeff_w = z->img_comp[i].w2 / 8;
      z->img_comp[i].coeff_h = z->img_comp[i].h2 / 8;
      z->img_comp[i].raw_coeff = stbi__malloc_mad3(
          z->img_comp[i].w2, z->img_comp[i].h2, sizeof(short), 15);
      if (z->img_comp[i].raw_coeff == NULL)
        return stbi__free_jpeg_components(
            z, i + 1, stbi__err("outofmem", "Out of memory"));
      z->img_comp[i].coeff =
          (short *)(((size_t)z->img_comp[i].raw_coeff + 15) & ~15);
    }
  }

  return 1;
}

// use comparisons since in some cases we handle more than one case (e.g. SOF)
#define stbi__DNL(x) ((x) == 0xdc)
#define stbi__SOI(x) ((x) == 0xd8)
#define stbi__EOI(x) ((x) == 0xd9)
#define stbi__SOF(x) ((x) == 0xc0 || (x) == 0xc1 || (x) == 0xc2)
#define stbi__SOS(x) ((x) == 0xda)

#define stbi__SOF_progressive(x) ((x) == 0xc2)

static int stbi__decode_jpeg_header(stbi__jpeg *z, int scan) {
  int m;
  z->jfif = 0;
  z->app14_color_transform = -1; // valid values are 0,1,2
  z->marker = STBI__MARKER_none; // initialize cached marker to empty
  m = stbi__get_marker(z);
  if (!stbi__SOI(m))
    return stbi__err("no SOI", "Corrupt JPEG");
  if (scan == STBI__SCAN_type)
    return 1;
  m = stbi__get_marker(z);
  while (!stbi__SOF(m)) {
    if (!stbi__process_marker(z, m))
      return 0;
    m = stbi__get_marker(z);
    while (m == STBI__MARKER_none) {
      // some files have extra padding after their blocks, so ok, we'll scan
      if (stbi__at_eof(z->s))
        return stbi__err("no SOF", "Corrupt JPEG");
      m = stbi__get_marker(z);
    }
  }
  z->progressive = stbi__SOF_progressive(m);
  if (!stbi__process_frame_header(z, scan))
    return 0;
  return 1;
}

// decode image to YCbCr format
static int stbi__decode_jpeg_image(stbi__jpeg *j) {
  int m;
  for (m = 0; m < 4; m++) {
    j->img_comp[m].raw_data = NULL;
    j->img_comp[m].raw_coeff = NULL;
  }
  j->restart_interval = 0;
  if (!stbi__decode_jpeg_header(j, STBI__SCAN_load))
    return 0;
  m = stbi__get_marker(j);
  while (!stbi__EOI(m)) {
    if (stbi__SOS(m)) {
      if (!stbi__process_scan_header(j))
        return 0;
      if (!stbi__parse_entropy_coded_data(j))
        return 0;
      if (j->marker == STBI__MARKER_none) {
        // handle 0s at the end of image data from IP Kamera 9060
        while (!stbi__at_eof(j->s)) {
          int x = stbi__get8(j->s);
          if (x == 255) {
            j->marker = stbi__get8(j->s);
            break;
          }
        }
        // if we reach eof without hitting a marker, stbi__get_marker() below
        // will fail and we'll eventually return 0
      }
    } else if (stbi__DNL(m)) {
      int Ld = stbi__get16be(j->s);
      stbi__uint32 NL = stbi__get16be(j->s);
      if (Ld != 4)
        stbi__err("bad DNL len", "Corrupt JPEG");
      if (NL != j->s->img_y)
        stbi__err("bad DNL height", "Corrupt JPEG");
    } else {
      if (!stbi__process_marker(j, m))
        return 0;
    }
    m = stbi__get_marker(j);
  }
  if (j->progressive)
    stbi__jpeg_finish(j);
  return 1;
}

// static jfif-centered resampling (across block boundaries)

typedef stbi_uc *(*resample_row_func)(stbi_uc *out, stbi_uc *in0, stbi_uc *in1,
                                      int w, int hs);

#define stbi__div4(x) ((stbi_uc)((x) >> 2))

static stbi_uc *resample_row_1(stbi_uc *out, stbi_uc *in_near, stbi_uc *in_far,
                               int w, int hs) {
  STBI_NOTUSED(out);
  STBI_NOTUSED(in_far);
  STBI_NOTUSED(w);
  STBI_NOTUSED(hs);
  return in_near;
}

static stbi_uc *stbi__resample_row_v_2(stbi_uc *out, stbi_uc *in_near,
                                       stbi_uc *in_far, int w, int hs) {
  // need to generate two samples vertically for every one in input
  int i;
  STBI_NOTUSED(hs);
  for (i = 0; i < w; ++i)
    out[i] = stbi__div4(3 * in_near[i] + in_far[i] + 2);
  return out;
}

static stbi_uc *stbi__resample_row_h_2(stbi_uc *out, stbi_uc *in_near,
                                       stbi_uc *in_far, int w, int hs) {
  // need to generate two samples horizontally for every one in input
  int i;
  stbi_uc *input = in_near;

  if (w == 1) {
    // if only one sample, can't do any interpolation
    out[0] = out[1] = input[0];
    return out;
  }

  out[0] = input[0];
  out[1] = stbi__div4(input[0] * 3 + input[1] + 2);
  for (i = 1; i < w - 1; ++i) {
    int n = 3 * input[i] + 2;
    out[i * 2 + 0] = stbi__div4(n + input[i - 1]);
    out[i * 2 + 1] = stbi__div4(n + input[i + 1]);
  }
  out[i * 2 + 0] = stbi__div4(input[w - 2] * 3 + input[w - 1] + 2);
  out[i * 2 + 1] = input[w - 1];

  STBI_NOTUSED(in_far);
  STBI_NOTUSED(hs);

  return out;
}

#define stbi__div16(x) ((stbi_uc)((x) >> 4))

static stbi_uc *stbi__resample_row_hv_2(stbi_uc *out, stbi_uc *in_near,
                                        stbi_uc *in_far, int w, int hs) {
  // need to generate 2x2 samples for every one in input
  int i, t0, t1;
  if (w == 1) {
    out[0] = out[1] = stbi__div4(3 * in_near[0] + in_far[0] + 2);
    return out;
  }

  t1 = 3 * in_near[0] + in_far[0];
  out[0] = stbi__div4(t1 + 2);
  for (i = 1; i < w; ++i) {
    t0 = t1;
    t1 = 3 * in_near[i] + in_far[i];
    out[i * 2 - 1] = stbi__div16(3 * t0 + t1 + 8);
    out[i * 2] = stbi__div16(3 * t1 + t0 + 8);
  }
  out[w * 2 - 1] = stbi__div4(t1 + 2);

  STBI_NOTUSED(hs);

  return out;
}

#if defined(STBI_SSE2) || defined(STBI_NEON)
static stbi_uc *stbi__resample_row_hv_2_simd(stbi_uc *out, stbi_uc *in_near,
                                             stbi_uc *in_far, int w, int hs) {
  // need to generate 2x2 samples for every one in input
  int i = 0, t0, t1;

  if (w == 1) {
    out[0] = out[1] = stbi__div4(3 * in_near[0] + in_far[0] + 2);
    return out;
  }

  t1 = 3 * in_near[0] + in_far[0];
  // process groups of 8 pixels for as long as we can.
  // note we can't handle the last pixel in a row in this loop
  // because we need to handle the filter boundary conditions.
  for (; i < ((w - 1) & ~7); i += 8) {
#if defined(STBI_SSE2)
    // load and perform the vertical filtering pass
    // this uses 3*x + y = 4*x + (y - x)
    __m128i zero = _mm_setzero_si128();
    __m128i farb = _mm_loadl_epi64((__m128i *)(in_far + i));
    __m128i nearb = _mm_loadl_epi64((__m128i *)(in_near + i));
    __m128i farw = _mm_unpacklo_epi8(farb, zero);
    __m128i nearw = _mm_unpacklo_epi8(nearb, zero);
    __m128i diff = _mm_sub_epi16(farw, nearw);
    __m128i nears = _mm_slli_epi16(nearw, 2);
    __m128i curr = _mm_add_epi16(nears, diff); // current row

    // horizontal filter works the same based on shifted vers of current
    // row. "prev" is current row shifted right by 1 pixel; we need to
    // insert the previous pixel value (from t1).
    // "next" is current row shifted left by 1 pixel, with first pixel
    // of next block of 8 pixels added in.
    __m128i prv0 = _mm_slli_si128(curr, 2);
    __m128i nxt0 = _mm_srli_si128(curr, 2);
    __m128i prev = _mm_insert_epi16(prv0, t1, 0);
    __m128i next =
        _mm_insert_epi16(nxt0, 3 * in_near[i + 8] + in_far[i + 8], 7);

    // horizontal filter, polyphase implementation since it's convenient:
    // even pixels = 3*cur + prev = cur*4 + (prev - cur)
    // odd  pixels = 3*cur + next = cur*4 + (next - cur)
    // note the shared term.
    __m128i bias = _mm_set1_epi16(8);
    __m128i curs = _mm_slli_epi16(curr, 2);
    __m128i prvd = _mm_sub_epi16(prev, curr);
    __m128i nxtd = _mm_sub_epi16(next, curr);
    __m128i curb = _mm_add_epi16(curs, bias);
    __m128i even = _mm_add_epi16(prvd, curb);
    __m128i odd = _mm_add_epi16(nxtd, curb);

    // interleave even and odd pixels, then undo scaling.
    __m128i int0 = _mm_unpacklo_epi16(even, odd);
    __m128i int1 = _mm_unpackhi_epi16(even, odd);
    __m128i de0 = _mm_srli_epi16(int0, 4);
    __m128i de1 = _mm_srli_epi16(int1, 4);

    // pack and write output
    __m128i outv = _mm_packus_epi16(de0, de1);
    _mm_storeu_si128((__m128i *)(out + i * 2), outv);
#elif defined(STBI_NEON)
    // load and perform the vertical filtering pass
    // this uses 3*x + y = 4*x + (y - x)
    uint8x8_t farb = vld1_u8(in_far + i);
    uint8x8_t nearb = vld1_u8(in_near + i);
    int16x8_t diff = vreinterpretq_s16_u16(vsubl_u8(farb, nearb));
    int16x8_t nears = vreinterpretq_s16_u16(vshll_n_u8(nearb, 2));
    int16x8_t curr = vaddq_s16(nears, diff); // current row

    // horizontal filter works the same based on shifted vers of current
    // row. "prev" is current row shifted right by 1 pixel; we need to
    // insert the previous pixel value (from t1).
    // "next" is current row shifted left by 1 pixel, with first pixel
    // of next block of 8 pixels added in.
    int16x8_t prv0 = vextq_s16(curr, curr, 7);
    int16x8_t nxt0 = vextq_s16(curr, curr, 1);
    int16x8_t prev = vsetq_lane_s16(t1, prv0, 0);
    int16x8_t next =
        vsetq_lane_s16(3 * in_near[i + 8] + in_far[i + 8], nxt0, 7);

    // horizontal filter, polyphase implementation since it's convenient:
    // even pixels = 3*cur + prev = cur*4 + (prev - cur)
    // odd  pixels = 3*cur + next = cur*4 + (next - cur)
    // note the shared term.
    int16x8_t curs = vshlq_n_s16(curr, 2);
    int16x8_t prvd = vsubq_s16(prev, curr);
    int16x8_t nxtd = vsubq_s16(next, curr);
    int16x8_t even = vaddq_s16(curs, prvd);
    int16x8_t odd = vaddq_s16(curs, nxtd);

    // undo scaling and round, then store with even/odd phases interleaved
    uint8x8x2_t o;
    o.val[0] = vqrshrun_n_s16(even, 4);
    o.val[1] = vqrshrun_n_s16(odd, 4);
    vst2_u8(out + i * 2, o);
#endif

    // "previous" value for next iter
    t1 = 3 * in_near[i + 7] + in_far[i + 7];
  }

  t0 = t1;
  t1 = 3 * in_near[i] + in_far[i];
  out[i * 2] = stbi__div16(3 * t1 + t0 + 8);

  for (++i; i < w; ++i) {
    t0 = t1;
    t1 = 3 * in_near[i] + in_far[i];
    out[i * 2 - 1] = stbi__div16(3 * t0 + t1 + 8);
    out[i * 2] = stbi__div16(3 * t1 + t0 + 8);
  }
  out[w * 2 - 1] = stbi__div4(t1 + 2);

  STBI_NOTUSED(hs);

  return out;
}
#endif

static stbi_uc *stbi__resample_row_generic(stbi_uc *out, stbi_uc *in_near,
                                           stbi_uc *in_far, int w, int hs) {
  // resample with nearest-neighbor
  int i, j;
  STBI_NOTUSED(in_far);
  for (i = 0; i < w; ++i)
    for (j = 0; j < hs; ++j)
      out[i * hs + j] = in_near[i];
  return out;
}

// this is a reduced-precision calculation of YCbCr-to-RGB introduced
// to make sure the code produces the same results in both SIMD and scalar
#define stbi__float2fixed(x) (((int)((x)*4096.0f + 0.5f)) << 8)
static void stbi__YCbCr_to_RGB_row(stbi_uc *out, const stbi_uc *y,
                                   const stbi_uc *pcb, const stbi_uc *pcr,
                                   int count, int step) {
  int i;
  for (i = 0; i < count; ++i) {
    int y_fixed = (y[i] << 20) + (1 << 19); // rounding
    int r, g, b;
    int cr = pcr[i] - 128;
    int cb = pcb[i] - 128;
    r = y_fixed + cr * stbi__float2fixed(1.40200f);
    g = y_fixed + (cr * -stbi__float2fixed(0.71414f)) +
        ((cb * -stbi__float2fixed(0.34414f)) & 0xffff0000);
    b = y_fixed + cb * stbi__float2fixed(1.77200f);
    r >>= 20;
    g >>= 20;
    b >>= 20;
    if ((unsigned)r > 255) {
      if (r < 0)
        r = 0;
      else
        r = 255;
    }
    if ((unsigned)g > 255) {
      if (g < 0)
        g = 0;
      else
        g = 255;
    }
    if ((unsigned)b > 255) {
      if (b < 0)
        b = 0;
      else
        b = 255;
    }
    out[0] = (stbi_uc)r;
    out[1] = (stbi_uc)g;
    out[2] = (stbi_uc)b;
    out[3] = 255;
    out += step;
  }
}

#if defined(STBI_SSE2) || defined(STBI_NEON)
static void stbi__YCbCr_to_RGB_simd(stbi_uc *out, stbi_uc const *y,
                                    stbi_uc const *pcb, stbi_uc const *pcr,
                                    int count, int step) {
  int i = 0;

#ifdef STBI_SSE2
  // step == 3 is pretty ugly on the final interleave, and i'm not convinced
  // it's useful in practice (you wouldn't use it for textures, for example).
  // so just accelerate step == 4 case.
  if (step == 4) {
    // this is a fairly straightforward implementation and not super-optimized.
    __m128i signflip = _mm_set1_epi8(-0x80);
    __m128i cr_const0 = _mm_set1_epi16((short)(1.40200f * 4096.0f + 0.5f));
    __m128i cr_const1 = _mm_set1_epi16(-(short)(0.71414f * 4096.0f + 0.5f));
    __m128i cb_const0 = _mm_set1_epi16(-(short)(0.34414f * 4096.0f + 0.5f));
    __m128i cb_const1 = _mm_set1_epi16((short)(1.77200f * 4096.0f + 0.5f));
    __m128i y_bias = _mm_set1_epi8((char)(unsigned char)128);
    __m128i xw = _mm_set1_epi16(255); // alpha channel

    for (; i + 7 < count; i += 8) {
      // load
      __m128i y_bytes = _mm_loadl_epi64((__m128i *)(y + i));
      __m128i cr_bytes = _mm_loadl_epi64((__m128i *)(pcr + i));
      __m128i cb_bytes = _mm_loadl_epi64((__m128i *)(pcb + i));
      __m128i cr_biased = _mm_xor_si128(cr_bytes, signflip); // -128
      __m128i cb_biased = _mm_xor_si128(cb_bytes, signflip); // -128

      // unpack to short (and left-shift cr, cb by 8)
      __m128i yw = _mm_unpacklo_epi8(y_bias, y_bytes);
      __m128i crw = _mm_unpacklo_epi8(_mm_setzero_si128(), cr_biased);
      __m128i cbw = _mm_unpacklo_epi8(_mm_setzero_si128(), cb_biased);

      // color transform
      __m128i yws = _mm_srli_epi16(yw, 4);
      __m128i cr0 = _mm_mulhi_epi16(cr_const0, crw);
      __m128i cb0 = _mm_mulhi_epi16(cb_const0, cbw);
      __m128i cb1 = _mm_mulhi_epi16(cbw, cb_const1);
      __m128i cr1 = _mm_mulhi_epi16(crw, cr_const1);
      __m128i rws = _mm_add_epi16(cr0, yws);
      __m128i gwt = _mm_add_epi16(cb0, yws);
      __m128i bws = _mm_add_epi16(yws, cb1);
      __m128i gws = _mm_add_epi16(gwt, cr1);

      // descale
      __m128i rw = _mm_srai_epi16(rws, 4);
      __m128i bw = _mm_srai_epi16(bws, 4);
      __m128i gw = _mm_srai_epi16(gws, 4);

      // back to byte, set up for transpose
      __m128i brb = _mm_packus_epi16(rw, bw);
      __m128i gxb = _mm_packus_epi16(gw, xw);

      // transpose to interleave channels
      __m128i t0 = _mm_unpacklo_epi8(brb, gxb);
      __m128i t1 = _mm_unpackhi_epi8(brb, gxb);
      __m128i o0 = _mm_unpacklo_epi16(t0, t1);
      __m128i o1 = _mm_unpackhi_epi16(t0, t1);

      // store
      _mm_storeu_si128((__m128i *)(out + 0), o0);
      _mm_storeu_si128((__m128i *)(out + 16), o1);
      out += 32;
    }
  }
#endif

#ifdef STBI_NEON
  // in this version, step=3 support would be easy to add. but is there demand?
  if (step == 4) {
    // this is a fairly straightforward implementation and not super-optimized.
    uint8x8_t signflip = vdup_n_u8(0x80);
    int16x8_t cr_const0 = vdupq_n_s16((short)(1.40200f * 4096.0f + 0.5f));
    int16x8_t cr_const1 = vdupq_n_s16(-(short)(0.71414f * 4096.0f + 0.5f));
    int16x8_t cb_const0 = vdupq_n_s16(-(short)(0.34414f * 4096.0f + 0.5f));
    int16x8_t cb_const1 = vdupq_n_s16((short)(1.77200f * 4096.0f + 0.5f));

    for (; i + 7 < count; i += 8) {
      // load
      uint8x8_t y_bytes = vld1_u8(y + i);
      uint8x8_t cr_bytes = vld1_u8(pcr + i);
      uint8x8_t cb_bytes = vld1_u8(pcb + i);
      int8x8_t cr_biased = vreinterpret_s8_u8(vsub_u8(cr_bytes, signflip));
      int8x8_t cb_biased = vreinterpret_s8_u8(vsub_u8(cb_bytes, signflip));

      // expand to s16
      int16x8_t yws = vreinterpretq_s16_u16(vshll_n_u8(y_bytes, 4));
      int16x8_t crw = vshll_n_s8(cr_biased, 7);
      int16x8_t cbw = vshll_n_s8(cb_biased, 7);

      // color transform
      int16x8_t cr0 = vqdmulhq_s16(crw, cr_const0);
      int16x8_t cb0 = vqdmulhq_s16(cbw, cb_const0);
      int16x8_t cr1 = vqdmulhq_s16(crw, cr_const1);
      int16x8_t cb1 = vqdmulhq_s16(cbw, cb_const1);
      int16x8_t rws = vaddq_s16(yws, cr0);
      int16x8_t gws = vaddq_s16(vaddq_s16(yws, cb0), cr1);
      int16x8_t bws = vaddq_s16(yws, cb1);

      // undo scaling, round, convert to byte
      uint8x8x4_t o;
      o.val[0] = vqrshrun_n_s16(rws, 4);
      o.val[1] = vqrshrun_n_s16(gws, 4);
      o.val[2] = vqrshrun_n_s16(bws, 4);
      o.val[3] = vdup_n_u8(255);

      // store, interleaving r/g/b/a
      vst4_u8(out, o);
      out += 8 * 4;
    }
  }
#endif

  for (; i < count; ++i) {
    int y_fixed = (y[i] << 20) + (1 << 19); // rounding
    int r, g, b;
    int cr = pcr[i] - 128;
    int cb = pcb[i] - 128;
    r = y_fixed + cr * stbi__float2fixed(1.40200f);
    g = y_fixed + cr * -stbi__float2fixed(0.71414f) +
        ((cb * -stbi__float2fixed(0.34414f)) & 0xffff0000);
    b = y_fixed + cb * stbi__float2fixed(1.77200f);
    r >>= 20;
    g >>= 20;
    b >>= 20;
    if ((unsigned)r > 255) {
      if (r < 0)
        r = 0;
      else
        r = 255;
    }
    if ((unsigned)g > 255) {
      if (g < 0)
        g = 0;
      else
        g = 255;
    }
    if ((unsigned)b > 255) {
      if (b < 0)
        b = 0;
      else
        b = 255;
    }
    out[0] = (stbi_uc)r;
    out[1] = (stbi_uc)g;
    out[2] = (stbi_uc)b;
    out[3] = 255;
    out += step;
  }
}
#endif

// set up the kernels
static void stbi__setup_jpeg(stbi__jpeg *j) {
  j->idct_block_kernel = stbi__idct_block;
  j->YCbCr_to_RGB_kernel = stbi__YCbCr_to_RGB_row;
  j->resample_row_hv_2_kernel = stbi__resample_row_hv_2;

#ifdef STBI_SSE2
  if (stbi__sse2_available()) {
    j->idct_block_kernel = stbi__idct_simd;
    j->YCbCr_to_RGB_kernel = stbi__YCbCr_to_RGB_simd;
    j->resample_row_hv_2_kernel = stbi__resample_row_hv_2_simd;
  }
#endif

#ifdef STBI_NEON
  j->idct_block_kernel = stbi__idct_simd;
  j->YCbCr_to_RGB_kernel = stbi__YCbCr_to_RGB_simd;
  j->resample_row_hv_2_kernel = stbi__resample_row_hv_2_simd;
#endif
}

// clean up the temporary component buffers
static void stbi__cleanup_jpeg(stbi__jpeg *j) {
  stbi__free_jpeg_components(j, j->s->img_n, 0);
}

typedef struct {
  resample_row_func resample;
  stbi_uc *line0, *line1;
  int hs, vs;  // expansion factor in each axis
  int w_lores; // horizontal pixels pre-expansion
  int ystep;   // how far through vertical expansion we are
  int ypos;    // which pre-expansion row we're on
} stbi__resample;

// fast 0..255 * 0..255 => 0..255 rounded multiplication
static stbi_uc stbi__blinn_8x8(stbi_uc x, stbi_uc y) {
  unsigned int t = x * y + 128;
  return (stbi_uc)((t + (t >> 8)) >> 8);
}

static stbi_uc *load_jpeg_image(stbi__jpeg *z, int *out_x, int *out_y,
                                int *comp, int req_comp) {
  int n, decode_n, is_rgb;
  z->s->img_n = 0; // make stbi__cleanup_jpeg safe

  // validate req_comp
  if (req_comp < 0 || req_comp > 4)
    return stbi__errpuc("bad req_comp", "Internal error");

  // load a jpeg image from whichever source, but leave in YCbCr format
  if (!stbi__decode_jpeg_image(z)) {
    stbi__cleanup_jpeg(z);
    return NULL;
  }

  // determine actual number of components to generate
  n = req_comp ? req_comp : z->s->img_n >= 3 ? 3 : 1;

  is_rgb = z->s->img_n == 3 &&
           (z->rgb == 3 || (z->app14_color_transform == 0 && !z->jfif));

  if (z->s->img_n == 3 && n < 3 && !is_rgb)
    decode_n = 1;
  else
    decode_n = z->s->img_n;

  // resample and color-convert
  {
    int k;
    unsigned int i, j;
    stbi_uc *output;
    stbi_uc *coutput[4];

    stbi__resample res_comp[4];

    for (k = 0; k < decode_n; ++k) {
      stbi__resample *r = &res_comp[k];

      // allocate line buffer big enough for upsampling off the edges
      // with upsample factor of 4
      z->img_comp[k].linebuf = (stbi_uc *)stbi__malloc(z->s->img_x + 3);
      if (!z->img_comp[k].linebuf) {
        stbi__cleanup_jpeg(z);
        return stbi__errpuc("outofmem", "Out of memory");
      }

      r->hs = z->img_h_max / z->img_comp[k].h;
      r->vs = z->img_v_max / z->img_comp[k].v;
      r->ystep = r->vs >> 1;
      r->w_lores = (z->s->img_x + r->hs - 1) / r->hs;
      r->ypos = 0;
      r->line0 = r->line1 = z->img_comp[k].data;

      if (r->hs == 1 && r->vs == 1)
        r->resample = resample_row_1;
      else if (r->hs == 1 && r->vs == 2)
        r->resample = stbi__resample_row_v_2;
      else if (r->hs == 2 && r->vs == 1)
        r->resample = stbi__resample_row_h_2;
      else if (r->hs == 2 && r->vs == 2)
        r->resample = z->resample_row_hv_2_kernel;
      else
        r->resample = stbi__resample_row_generic;
    }

    // can't error after this so, this is safe
    output = (stbi_uc *)stbi__malloc_mad3(n, z->s->img_x, z->s->img_y, 1);
    if (!output) {
      stbi__cleanup_jpeg(z);
      return stbi__errpuc("outofmem", "Out of memory");
    }

    // now go ahead and resample
    for (j = 0; j < z->s->img_y; ++j) {
      stbi_uc *out = output + n * z->s->img_x * j;
      for (k = 0; k < decode_n; ++k) {
        stbi__resample *r = &res_comp[k];
        int y_bot = r->ystep >= (r->vs >> 1);
        coutput[k] =
            r->resample(z->img_comp[k].linebuf, y_bot ? r->line1 : r->line0,
                        y_bot ? r->line0 : r->line1, r->w_lores, r->hs);
        if (++r->ystep >= r->vs) {
          r->ystep = 0;
          r->line0 = r->line1;
          if (++r->ypos < z->img_comp[k].y)
            r->line1 += z->img_comp[k].w2;
        }
      }
      if (n >= 3) {
        stbi_uc *y = coutput[0];
        if (z->s->img_n == 3) {
          if (is_rgb) {
            for (i = 0; i < z->s->img_x; ++i) {
              out[0] = y[i];
              out[1] = coutput[1][i];
              out[2] = coutput[2][i];
              out[3] = 255;
              out += n;
            }
          } else {
            z->YCbCr_to_RGB_kernel(out, y, coutput[1], coutput[2], z->s->img_x,
                                   n);
          }
        } else if (z->s->img_n == 4) {
          if (z->app14_color_transform == 0) { // CMYK
            for (i = 0; i < z->s->img_x; ++i) {
              stbi_uc m = coutput[3][i];
              out[0] = stbi__blinn_8x8(coutput[0][i], m);
              out[1] = stbi__blinn_8x8(coutput[1][i], m);
              out[2] = stbi__blinn_8x8(coutput[2][i], m);
              out[3] = 255;
              out += n;
            }
          } else if (z->app14_color_transform == 2) { // YCCK
            z->YCbCr_to_RGB_kernel(out, y, coutput[1], coutput[2], z->s->img_x,
                                   n);
            for (i = 0; i < z->s->img_x; ++i) {
              stbi_uc m = coutput[3][i];
              out[0] = stbi__blinn_8x8(255 - out[0], m);
              out[1] = stbi__blinn_8x8(255 - out[1], m);
              out[2] = stbi__blinn_8x8(255 - out[2], m);
              out += n;
            }
          } else { // YCbCr + alpha?  Ignore the fourth channel for now
            z->YCbCr_to_RGB_kernel(out, y, coutput[1], coutput[2], z->s->img_x,
                                   n);
          }
        } else
          for (i = 0; i < z->s->img_x; ++i) {
            out[0] = out[1] = out[2] = y[i];
            out[3] = 255; // not used if n==3
            out += n;
          }
      } else {
        if (is_rgb) {
          if (n == 1)
            for (i = 0; i < z->s->img_x; ++i)
              *out++ =
                  stbi__compute_y(coutput[0][i], coutput[1][i], coutput[2][i]);
          else {
            for (i = 0; i < z->s->img_x; ++i, out += 2) {
              out[0] =
                  stbi__compute_y(coutput[0][i], coutput[1][i], coutput[2][i]);
              out[1] = 255;
            }
          }
        } else if (z->s->img_n == 4 && z->app14_color_transform == 0) {
          for (i = 0; i < z->s->img_x; ++i) {
            stbi_uc m = coutput[3][i];
            stbi_uc r = stbi__blinn_8x8(coutput[0][i], m);
            stbi_uc g = stbi__blinn_8x8(coutput[1][i], m);
            stbi_uc b = stbi__blinn_8x8(coutput[2][i], m);
            out[0] = stbi__compute_y(r, g, b);
            out[1] = 255;
            out += n;
          }
        } else if (z->s->img_n == 4 && z->app14_color_transform == 2) {
          for (i = 0; i < z->s->img_x; ++i) {
            out[0] = stbi__blinn_8x8(255 - coutput[0][i], coutput[3][i]);
            out[1] = 255;
            out += n;
          }
        } else {
          stbi_uc *y = coutput[0];
          if (n == 1)
            for (i = 0; i < z->s->img_x; ++i)
              out[i] = y[i];
          else
            for (i = 0; i < z->s->img_x; ++i)
              *out++ = y[i], *out++ = 255;
        }
      }
    }
    stbi__cleanup_jpeg(z);
    *out_x = z->s->img_x;
    *out_y = z->s->img_y;
    if (comp)
      *comp =
          z->s->img_n >= 3 ? 3 : 1; // report original components, not output
    return output;
  }
}

static void *stbi__jpeg_load(stbi__context *s, int *x, int *y, int *comp,
                             int req_comp, stbi__result_info *ri) {
  unsigned char *result;
  stbi__jpeg *j = (stbi__jpeg *)stbi__malloc(sizeof(stbi__jpeg));
  STBI_NOTUSED(ri);
  j->s = s;
  stbi__setup_jpeg(j);
  result = load_jpeg_image(j, x, y, comp, req_comp);
  STBI_FREE(j);
  return result;
}

static int stbi__jpeg_test(stbi__context *s) {
  int r;
  stbi__jpeg *j = (stbi__jpeg *)stbi__malloc(sizeof(stbi__jpeg));
  j->s = s;
  stbi__setup_jpeg(j);
  r = stbi__decode_jpeg_header(j, STBI__SCAN_type);
  stbi__rewind(s);
  STBI_FREE(j);
  return r;
}

static int stbi__jpeg_info_raw(stbi__jpeg *j, int *x, int *y, int *comp) {
  if (!stbi__decode_jpeg_header(j, STBI__SCAN_header)) {
    stbi__rewind(j->s);
    return 0;
  }
  if (x)
    *x = j->s->img_x;
  if (y)
    *y = j->s->img_y;
  if (comp)
    *comp = j->s->img_n >= 3 ? 3 : 1;
  return 1;
}

static int stbi__jpeg_info(stbi__context *s, int *x, int *y, int *comp) {
  int result;
  stbi__jpeg *j = (stbi__jpeg *)(stbi__malloc(sizeof(stbi__jpeg)));
  j->s = s;
  result = stbi__jpeg_info_raw(j, x, y, comp);
  STBI_FREE(j);
  return result;
}
#endif

  // public domain zlib decode    v0.2  Sean Barrett 2006-11-18
  //    simple implementation
  //      - all input must be provided in an upfront buffer
  //      - all output is written to a single output buffer (can malloc/realloc)
  //    performance
  //      - fast huffman

#ifndef STBI_NO_ZLIB

// fast-way is faster to check than jpeg huffman, but slow way is slower
#define STBI__ZFAST_BITS 9 // accelerate all cases in default tables
#define STBI__ZFAST_MASK ((1 << STBI__ZFAST_BITS) - 1)

// zlib-style huffman encoding
// (jpegs packs from left, zlib from right, so can't share code)
typedef struct {
  stbi__uint16 fast[1 << STBI__ZFAST_BITS];
  stbi__uint16 firstcode[16];
  int maxcode[17];
  stbi__uint16 firstsymbol[16];
  stbi_uc size[288];
  stbi__uint16 value[288];
} stbi__zhuffman;

stbi_inline static int stbi__bitreverse16(int n) {
  n = ((n & 0xAAAA) >> 1) | ((n & 0x5555) << 1);
  n = ((n & 0xCCCC) >> 2) | ((n & 0x3333) << 2);
  n = ((n & 0xF0F0) >> 4) | ((n & 0x0F0F) << 4);
  n = ((n & 0xFF00) >> 8) | ((n & 0x00FF) << 8);
  return n;
}

stbi_inline static int stbi__bit_reverse(int v, int bits) {
  STBI_ASSERT(bits <= 16);
  // to bit reverse n bits, reverse 16 and shift
  // e.g. 11 bits, bit reverse and shift away 5
  return stbi__bitreverse16(v) >> (16 - bits);
}

static int stbi__zbuild_huffman(stbi__zhuffman *z, const stbi_uc *sizelist,
                                int num) {
  int i, k = 0;
  int code, next_code[16], sizes[17];

  // DEFLATE spec for generating codes
  memset(sizes, 0, sizeof(sizes));
  memset(z->fast, 0, sizeof(z->fast));
  for (i = 0; i < num; ++i)
    ++sizes[sizelist[i]];
  sizes[0] = 0;
  for (i = 1; i < 16; ++i)
    if (sizes[i] > (1 << i))
      return stbi__err("bad sizes", "Corrupt PNG");
  code = 0;
  for (i = 1; i < 16; ++i) {
    next_code[i] = code;
    z->firstcode[i] = (stbi__uint16)code;
    z->firstsymbol[i] = (stbi__uint16)k;
    code = (code + sizes[i]);
    if (sizes[i])
      if (code - 1 >= (1 << i))
        return stbi__err("bad codelengths", "Corrupt PNG");
    z->maxcode[i] = code << (16 - i); // preshift for inner loop
    code <<= 1;
    k += sizes[i];
  }
  z->maxcode[16] = 0x10000; // sentinel
  for (i = 0; i < num; ++i) {
    int s = sizelist[i];
    if (s) {
      int c = next_code[s] - z->firstcode[s] + z->firstsymbol[s];
      stbi__uint16 fastv = (stbi__uint16)((s << 9) | i);
      z->size[c] = (stbi_uc)s;
      z->value[c] = (stbi__uint16)i;
      if (s <= STBI__ZFAST_BITS) {
        int j = stbi__bit_reverse(next_code[s], s);
        while (j < (1 << STBI__ZFAST_BITS)) {
          z->fast[j] = fastv;
          j += (1 << s);
        }
      }
      ++next_code[s];
    }
  }
  return 1;
}

// zlib-from-memory implementation for PNG reading
//    because PNG allows splitting the zlib stream arbitrarily,
//    and it's annoying structurally to have PNG call ZLIB call PNG,
//    we require PNG read all the IDATs and combine them into a single
//    memory buffer

typedef struct {
  stbi_uc *zbuffer, *zbuffer_end;
  int num_bits;
  stbi__uint32 code_buffer;

  char *zout;
  char *zout_start;
  char *zout_end;
  int z_expandable;

  stbi__zhuffman z_length, z_distance;
} stbi__zbuf;

stbi_inline static stbi_uc stbi__zget8(stbi__zbuf *z) {
  if (z->zbuffer >= z->zbuffer_end)
    return 0;
  return *z->zbuffer++;
}

static void stbi__fill_bits(stbi__zbuf *z) {
  do {
    STBI_ASSERT(z->code_buffer < (1U << z->num_bits));
    z->code_buffer |= (unsigned int)stbi__zget8(z) << z->num_bits;
    z->num_bits += 8;
  } while (z->num_bits <= 24);
}

stbi_inline static unsigned int stbi__zreceive(stbi__zbuf *z, int n) {
  unsigned int k;
  if (z->num_bits < n)
    stbi__fill_bits(z);
  k = z->code_buffer & ((1 << n) - 1);
  z->code_buffer >>= n;
  z->num_bits -= n;
  return k;
}

static int stbi__zhuffman_decode_slowpath(stbi__zbuf *a, stbi__zhuffman *z) {
  int b, s, k;
  // not resolved by fast table, so compute it the slow way
  // use jpeg approach, which requires MSbits at top
  k = stbi__bit_reverse(a->code_buffer, 16);
  for (s = STBI__ZFAST_BITS + 1;; ++s)
    if (k < z->maxcode[s])
      break;
  if (s == 16)
    return -1; // invalid code!
  // code size is s, so:
  b = (k >> (16 - s)) - z->firstcode[s] + z->firstsymbol[s];
  STBI_ASSERT(z->size[b] == s);
  a->code_buffer >>= s;
  a->num_bits -= s;
  return z->value[b];
}

stbi_inline static int stbi__zhuffman_decode(stbi__zbuf *a, stbi__zhuffman *z) {
  int b, s;
  if (a->num_bits < 16)
    stbi__fill_bits(a);
  b = z->fast[a->code_buffer & STBI__ZFAST_MASK];
  if (b) {
    s = b >> 9;
    a->code_buffer >>= s;
    a->num_bits -= s;
    return b & 511;
  }
  return stbi__zhuffman_decode_slowpath(a, z);
}

static int stbi__zexpand(stbi__zbuf *z, char *zout,
                         int n) // need to make room for n bytes
{
  char *q;
  int cur, limit, old_limit;
  z->zout = zout;
  if (!z->z_expandable)
    return stbi__err("output buffer limit", "Corrupt PNG");
  cur = (int)(z->zout - z->zout_start);
  limit = old_limit = (int)(z->zout_end - z->zout_start);
  while (cur + n > limit)
    limit *= 2;
  q = (char *)STBI_REALLOC_SIZED(z->zout_start, old_limit, limit);
  STBI_NOTUSED(old_limit);
  if (q == NULL)
    return stbi__err("outofmem", "Out of memory");
  z->zout_start = q;
  z->zout = q + cur;
  z->zout_end = q + limit;
  return 1;
}

static int stbi__zlength_base[31] = {
    3,  4,  5,  6,  7,  8,  9,  10,  11,  13,  15,  17,  19,  23, 27, 31,
    35, 43, 51, 59, 67, 83, 99, 115, 131, 163, 195, 227, 258, 0,  0};

static int stbi__zlength_extra[31] = {0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1,
                                      1, 2, 2, 2, 2, 3, 3, 3, 3, 4, 4,
                                      4, 4, 5, 5, 5, 5, 0, 0, 0};

static int stbi__zdist_base[32] = {
    1,    2,    3,    4,    5,    7,     9,     13,    17,  25,   33,
    49,   65,   97,   129,  193,  257,   385,   513,   769, 1025, 1537,
    2049, 3073, 4097, 6145, 8193, 12289, 16385, 24577, 0,   0};

static int stbi__zdist_extra[32] = {0, 0, 0,  0,  1,  1,  2,  2,  3,  3,
                                    4, 4, 5,  5,  6,  6,  7,  7,  8,  8,
                                    9, 9, 10, 10, 11, 11, 12, 12, 13, 13};

static int stbi__parse_huffman_block(stbi__zbuf *a) {
  char *zout = a->zout;
  for (;;) {
    int z = stbi__zhuffman_decode(a, &a->z_length);
    if (z < 256) {
      if (z < 0)
        return stbi__err("bad huffman code",
                         "Corrupt PNG"); // error in huffman codes
      if (zout >= a->zout_end) {
        if (!stbi__zexpand(a, zout, 1))
          return 0;
        zout = a->zout;
      }
      *zout++ = (char)z;
    } else {
      stbi_uc *p;
      int len, dist;
      if (z == 256) {
        a->zout = zout;
        return 1;
      }
      z -= 257;
      len = stbi__zlength_base[z];
      if (stbi__zlength_extra[z])
        len += stbi__zreceive(a, stbi__zlength_extra[z]);
      z = stbi__zhuffman_decode(a, &a->z_distance);
      if (z < 0)
        return stbi__err("bad huffman code", "Corrupt PNG");
      dist = stbi__zdist_base[z];
      if (stbi__zdist_extra[z])
        dist += stbi__zreceive(a, stbi__zdist_extra[z]);
      if (zout - a->zout_start < dist)
        return stbi__err("bad dist", "Corrupt PNG");
      if (zout + len > a->zout_end) {
        if (!stbi__zexpand(a, zout, len))
          return 0;
        zout = a->zout;
      }
      p = (stbi_uc *)(zout - dist);
      if (dist == 1) { // run of one byte; common in images.
        stbi_uc v = *p;
        if (len) {
          do
            *zout++ = v;
          while (--len);
        }
      } else {
        if (len) {
          do
            *zout++ = *p++;
          while (--len);
        }
      }
    }
  }
}

static int stbi__compute_huffman_codes(stbi__zbuf *a) {
  static stbi_uc length_dezigzag[19] = {16, 17, 18, 0, 8,  7, 9,  6, 10, 5,
                                        11, 4,  12, 3, 13, 2, 14, 1, 15};
  stbi__zhuffman z_codelength;
  stbi_uc lencodes[286 + 32 + 137]; // padding for maximum single op
  stbi_uc codelength_sizes[19];
  int i, n;

  int hlit = stbi__zreceive(a, 5) + 257;
  int hdist = stbi__zreceive(a, 5) + 1;
  int hclen = stbi__zreceive(a, 4) + 4;
  int ntot = hlit + hdist;

  memset(codelength_sizes, 0, sizeof(codelength_sizes));
  for (i = 0; i < hclen; ++i) {
    int s = stbi__zreceive(a, 3);
    codelength_sizes[length_dezigzag[i]] = (stbi_uc)s;
  }
  if (!stbi__zbuild_huffman(&z_codelength, codelength_sizes, 19))
    return 0;

  n = 0;
  while (n < ntot) {
    int c = stbi__zhuffman_decode(a, &z_codelength);
    if (c < 0 || c >= 19)
      return stbi__err("bad codelengths", "Corrupt PNG");
    if (c < 16)
      lencodes[n++] = (stbi_uc)c;
    else {
      stbi_uc fill = 0;
      if (c == 16) {
        c = stbi__zreceive(a, 2) + 3;
        if (n == 0)
          return stbi__err("bad codelengths", "Corrupt PNG");
        fill = lencodes[n - 1];
      } else if (c == 17)
        c = stbi__zreceive(a, 3) + 3;
      else {
        STBI_ASSERT(c == 18);
        c = stbi__zreceive(a, 7) + 11;
      }
      if (ntot - n < c)
        return stbi__err("bad codelengths", "Corrupt PNG");
      memset(lencodes + n, fill, c);
      n += c;
    }
  }
  if (n != ntot)
    return stbi__err("bad codelengths", "Corrupt PNG");
  if (!stbi__zbuild_huffman(&a->z_length, lencodes, hlit))
    return 0;
  if (!stbi__zbuild_huffman(&a->z_distance, lencodes + hlit, hdist))
    return 0;
  return 1;
}

static int stbi__parse_uncompressed_block(stbi__zbuf *a) {
  stbi_uc header[4];
  int len, nlen, k;
  if (a->num_bits & 7)
    stbi__zreceive(a, a->num_bits & 7); // discard
  // drain the bit-packed data into header
  k = 0;
  while (a->num_bits > 0) {
    header[k++] =
        (stbi_uc)(a->code_buffer & 255); // suppress MSVC run-time check
    a->code_buffer >>= 8;
    a->num_bits -= 8;
  }
  STBI_ASSERT(a->num_bits == 0);
  // now fill header the normal way
  while (k < 4)
    header[k++] = stbi__zget8(a);
  len = header[1] * 256 + header[0];
  nlen = header[3] * 256 + header[2];
  if (nlen != (len ^ 0xffff))
    return stbi__err("zlib corrupt", "Corrupt PNG");
  if (a->zbuffer + len > a->zbuffer_end)
    return stbi__err("read past buffer", "Corrupt PNG");
  if (a->zout + len > a->zout_end)
    if (!stbi__zexpand(a, a->zout, len))
      return 0;
  memcpy(a->zout, a->zbuffer, len);
  a->zbuffer += len;
  a->zout += len;
  return 1;
}

static int stbi__parse_zlib_header(stbi__zbuf *a) {
  int cmf = stbi__zget8(a);
  int cm = cmf & 15;
  /* int cinfo = cmf >> 4; */
  int flg = stbi__zget8(a);
  if ((cmf * 256 + flg) % 31 != 0)
    return stbi__err("bad zlib header", "Corrupt PNG"); // zlib spec
  if (flg & 32)
    return stbi__err("no preset dict",
                     "Corrupt PNG"); // preset dictionary not allowed in png
  if (cm != 8)
    return stbi__err("bad compression",
                     "Corrupt PNG"); // DEFLATE required for png
  // window = 1 << (8 + cinfo)... but who cares, we fully buffer output
  return 1;
}

static const stbi_uc stbi__zdefault_length[288] = {
    8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8,
    8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8,
    8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8,
    8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8,
    8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8,
    8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8,
    9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9,
    9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9,
    9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9,
    9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9,
    9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 7, 7, 7, 7, 7, 7, 7, 7,
    7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 8, 8, 8, 8, 8, 8, 8, 8};
static const stbi_uc stbi__zdefault_distance[32] = {
    5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5,
    5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5};
/*
Init algorithm:
{
   int i;   // use <= to match clearly with spec
   for (i=0; i <= 143; ++i)     stbi__zdefault_length[i]   = 8;
   for (   ; i <= 255; ++i)     stbi__zdefault_length[i]   = 9;
   for (   ; i <= 279; ++i)     stbi__zdefault_length[i]   = 7;
   for (   ; i <= 287; ++i)     stbi__zdefault_length[i]   = 8;

   for (i=0; i <=  31; ++i)     stbi__zdefault_distance[i] = 5;
}
*/

static int stbi__parse_zlib(stbi__zbuf *a, int parse_header) {
  int final, type;
  if (parse_header)
    if (!stbi__parse_zlib_header(a))
      return 0;
  a->num_bits = 0;
  a->code_buffer = 0;
  do {
    final = stbi__zreceive(a, 1);
    type = stbi__zreceive(a, 2);
    if (type == 0) {
      if (!stbi__parse_uncompressed_block(a))
        return 0;
    } else if (type == 3) {
      return 0;
    } else {
      if (type == 1) {
        // use fixed code lengths
        if (!stbi__zbuild_huffman(&a->z_length, stbi__zdefault_length, 288))
          return 0;
        if (!stbi__zbuild_huffman(&a->z_distance, stbi__zdefault_distance, 32))
          return 0;
      } else {
        if (!stbi__compute_huffman_codes(a))
          return 0;
      }
      if (!stbi__parse_huffman_block(a))
        return 0;
    }
  } while (!final);
  return 1;
}

static int stbi__do_zlib(stbi__zbuf *a, char *obuf, int olen, int exp,
                         int parse_header) {
  a->zout_start = obuf;
  a->zout = obuf;
  a->zout_end = obuf + olen;
  a->z_expandable = exp;

  return stbi__parse_zlib(a, parse_header);
}

STBIDEF char *stbi_zlib_decode_malloc_guesssize(const char *buffer, int len,
                                                int initial_size, int *outlen) {
  stbi__zbuf a;
  char *p = (char *)stbi__malloc(initial_size);
  if (p == NULL)
    return NULL;
  a.zbuffer = (stbi_uc *)buffer;
  a.zbuffer_end = (stbi_uc *)buffer + len;
  if (stbi__do_zlib(&a, p, initial_size, 1, 1)) {
    if (outlen)
      *outlen = (int)(a.zout - a.zout_start);
    return a.zout_start;
  } else {
    STBI_FREE(a.zout_start);
    return NULL;
  }
}

STBIDEF char *stbi_zlib_decode_malloc(char const *buffer, int len,
                                      int *outlen) {
  return stbi_zlib_decode_malloc_guesssize(buffer, len, 16384, outlen);
}

STBIDEF char *stbi_zlib_decode_malloc_guesssize_headerflag(const char *buffer,
                                                           int len,
                                                           int initial_size,
                                                           int *outlen,
                                                           int parse_header) {
  stbi__zbuf a;
  char *p = (char *)stbi__malloc(initial_size);
  if (p == NULL)
    return NULL;
  a.zbuffer = (stbi_uc *)buffer;
  a.zbuffer_end = (stbi_uc *)buffer + len;
  if (stbi__do_zlib(&a, p, initial_size, 1, parse_header)) {
    if (outlen)
      *outlen = (int)(a.zout - a.zout_start);
    return a.zout_start;
  } else {
    STBI_FREE(a.zout_start);
    return NULL;
  }
}

STBIDEF int stbi_zlib_decode_buffer(char *obuffer, int olen,
                                    char const *ibuffer, int ilen) {
  stbi__zbuf a;
  a.zbuffer = (stbi_uc *)ibuffer;
  a.zbuffer_end = (stbi_uc *)ibuffer + ilen;
  if (stbi__do_zlib(&a, obuffer, olen, 0, 1))
    return (int)(a.zout - a.zout_start);
  else
    return -1;
}

STBIDEF char *stbi_zlib_decode_noheader_malloc(char const *buffer, int len,
                                               int *outlen) {
  stbi__zbuf a;
  char *p = (char *)stbi__malloc(16384);
  if (p == NULL)
    return NULL;
  a.zbuffer = (stbi_uc *)buffer;
  a.zbuffer_end = (stbi_uc *)buffer + len;
  if (stbi__do_zlib(&a, p, 16384, 1, 0)) {
    if (outlen)
      *outlen = (int)(a.zout - a.zout_start);
    return a.zout_start;
  } else {
    STBI_FREE(a.zout_start);
    return NULL;
  }
}

STBIDEF int stbi_zlib_decode_noheader_buffer(char *obuffer, int olen,
                                             const char *ibuffer, int ilen) {
  stbi__zbuf a;
  a.zbuffer = (stbi_uc *)ibuffer;
  a.zbuffer_end = (stbi_uc *)ibuffer + ilen;
  if (stbi__do_zlib(&a, obuffer, olen, 0, 0))
    return (int)(a.zout - a.zout_start);
  else
    return -1;
}
#endif

  // public domain "baseline" PNG decoder   v0.10  Sean Barrett 2006-11-18
  //    simple implementation
  //      - only 8-bit samples
  //      - no CRC checking
  //      - allocates lots of intermediate memory
  //        - avoids problem of streaming data between subsystems
  //        - avoids explicit window management
  //    performance
  //      - uses stb_zlib, a PD zlib implementation with fast huffman decoding

#ifndef STBI_NO_PNG
typedef struct {
  stbi__uint32 length;
  stbi__uint32 type;
} stbi__pngchunk;

static stbi__pngchunk stbi__get_chunk_header(stbi__context *s) {
  stbi__pngchunk c;
  c.length = stbi__get32be(s);
  c.type = stbi__get32be(s);
  return c;
}

static int stbi__check_png_header(stbi__context *s) {
  static stbi_uc png_sig[8] = {137, 80, 78, 71, 13, 10, 26, 10};
  int i;
  for (i = 0; i < 8; ++i)
    if (stbi__get8(s) != png_sig[i])
      return stbi__err("bad png sig", "Not a PNG");
  return 1;
}

typedef struct {
  stbi__context *s;
  stbi_uc *idata, *expanded, *out;
  int depth;
} stbi__png;

enum {
  STBI__F_none = 0,
  STBI__F_sub = 1,
  STBI__F_up = 2,
  STBI__F_avg = 3,
  STBI__F_paeth = 4,
  // synthetic filters used for first scanline to avoid needing a dummy row of
  // 0s
  STBI__F_avg_first,
  STBI__F_paeth_first
};

static stbi_uc first_row_filter[5] = {STBI__F_none, STBI__F_sub, STBI__F_none,
                                      STBI__F_avg_first, STBI__F_paeth_first};

static int stbi__paeth(int a, int b, int c) {
  int p = a + b - c;
  int pa = abs(p - a);
  int pb = abs(p - b);
  int pc = abs(p - c);
  if (pa <= pb && pa <= pc)
    return a;
  if (pb <= pc)
    return b;
  return c;
}

static stbi_uc stbi__depth_scale_table[9] = {0, 0xff, 0x55, 0,   0x11,
                                             0, 0,    0,    0x01};

// create the png data from post-deflated data
static int stbi__create_png_image_raw(stbi__png *a, stbi_uc *raw,
                                      stbi__uint32 raw_len, int out_n,
                                      stbi__uint32 x, stbi__uint32 y, int depth,
                                      int color) {
  int bytes = (depth == 16 ? 2 : 1);
  stbi__context *s = a->s;
  stbi__uint32 i, j, stride = x * out_n * bytes;
  stbi__uint32 img_len, img_width_bytes;
  int k;
  int img_n = s->img_n; // copy it into a local for later

  int output_bytes = out_n * bytes;
  int filter_bytes = img_n * bytes;
  int width = x;

  STBI_ASSERT(out_n == s->img_n || out_n == s->img_n + 1);
  a->out = (stbi_uc *)stbi__malloc_mad3(
      x, y, output_bytes, 0); // extra bytes to write off the end into
  if (!a->out)
    return stbi__err("outofmem", "Out of memory");

  img_width_bytes = (((img_n * x * depth) + 7) >> 3);
  img_len = (img_width_bytes + 1) * y;
  // we used to check for exact match between raw_len and img_len on
  // non-interlaced PNGs, but issue #276 reported a PNG in the wild that had
  // extra data at the end (all zeros), so just check for raw_len < img_len
  // always.
  if (raw_len < img_len)
    return stbi__err("not enough pixels", "Corrupt PNG");

  for (j = 0; j < y; ++j) {
    stbi_uc *cur = a->out + stride * j;
    stbi_uc *prior;
    int filter = *raw++;

    if (filter > 4)
      return stbi__err("invalid filter", "Corrupt PNG");

    if (depth < 8) {
      STBI_ASSERT(img_width_bytes <= x);
      cur += x * out_n - img_width_bytes; // store output to the rightmost
                                          // img_len bytes, so we can decode in
                                          // place
      filter_bytes = 1;
      width = img_width_bytes;
    }
    prior =
        cur -
        stride; // bugfix: need to compute this after 'cur +=' computation above

    // if first row, use special filter that doesn't sample previous row
    if (j == 0)
      filter = first_row_filter[filter];

    // handle first byte explicitly
    for (k = 0; k < filter_bytes; ++k) {
      switch (filter) {
      case STBI__F_none:
        cur[k] = raw[k];
        break;
      case STBI__F_sub:
        cur[k] = raw[k];
        break;
      case STBI__F_up:
        cur[k] = STBI__BYTECAST(raw[k] + prior[k]);
        break;
      case STBI__F_avg:
        cur[k] = STBI__BYTECAST(raw[k] + (prior[k] >> 1));
        break;
      case STBI__F_paeth:
        cur[k] = STBI__BYTECAST(raw[k] + stbi__paeth(0, prior[k], 0));
        break;
      case STBI__F_avg_first:
        cur[k] = raw[k];
        break;
      case STBI__F_paeth_first:
        cur[k] = raw[k];
        break;
      }
    }

    if (depth == 8) {
      if (img_n != out_n)
        cur[img_n] = 255; // first pixel
      raw += img_n;
      cur += out_n;
      prior += out_n;
    } else if (depth == 16) {
      if (img_n != out_n) {
        cur[filter_bytes] = 255;     // first pixel top byte
        cur[filter_bytes + 1] = 255; // first pixel bottom byte
      }
      raw += filter_bytes;
      cur += output_bytes;
      prior += output_bytes;
    } else {
      raw += 1;
      cur += 1;
      prior += 1;
    }

    // this is a little gross, so that we don't switch per-pixel or
    // per-component
    if (depth < 8 || img_n == out_n) {
      int nk = (width - 1) * filter_bytes;
#define STBI__CASE(f)                                                          \
  case f:                                                                      \
    for (k = 0; k < nk; ++k)
      switch (filter) {
      // "none" filter turns into a memcpy here; make that explicit.
      case STBI__F_none:
        memcpy(cur, raw, nk);
        break;
        STBI__CASE(STBI__F_sub) {
          cur[k] = STBI__BYTECAST(raw[k] + cur[k - filter_bytes]);
        }
        break;
        STBI__CASE(STBI__F_up) { cur[k] = STBI__BYTECAST(raw[k] + prior[k]); }
        break;
        STBI__CASE(STBI__F_avg) {
          cur[k] = STBI__BYTECAST(raw[k] +
                                  ((prior[k] + cur[k - filter_bytes]) >> 1));
        }
        break;
        STBI__CASE(STBI__F_paeth) {
          cur[k] = STBI__BYTECAST(raw[k] +
                                  stbi__paeth(cur[k - filter_bytes], prior[k],
                                              prior[k - filter_bytes]));
        }
        break;
        STBI__CASE(STBI__F_avg_first) {
          cur[k] = STBI__BYTECAST(raw[k] + (cur[k - filter_bytes] >> 1));
        }
        break;
        STBI__CASE(STBI__F_paeth_first) {
          cur[k] =
              STBI__BYTECAST(raw[k] + stbi__paeth(cur[k - filter_bytes], 0, 0));
        }
        break;
      }
#undef STBI__CASE
      raw += nk;
    } else {
      STBI_ASSERT(img_n + 1 == out_n);
#define STBI__CASE(f)                                                          \
  case f:                                                                      \
    for (i = x - 1; i >= 1; --i, cur[filter_bytes] = 255, raw += filter_bytes, \
        cur += output_bytes, prior += output_bytes)                            \
      for (k = 0; k < filter_bytes; ++k)
      switch (filter) {
        STBI__CASE(STBI__F_none) { cur[k] = raw[k]; }
        break;
        STBI__CASE(STBI__F_sub) {
          cur[k] = STBI__BYTECAST(raw[k] + cur[k - output_bytes]);
        }
        break;
        STBI__CASE(STBI__F_up) { cur[k] = STBI__BYTECAST(raw[k] + prior[k]); }
        break;
        STBI__CASE(STBI__F_avg) {
          cur[k] = STBI__BYTECAST(raw[k] +
                                  ((prior[k] + cur[k - output_bytes]) >> 1));
        }
        break;
        STBI__CASE(STBI__F_paeth) {
          cur[k] = STBI__BYTECAST(raw[k] +
                                  stbi__paeth(cur[k - output_bytes], prior[k],
                                              prior[k - output_bytes]));
        }
        break;
        STBI__CASE(STBI__F_avg_first) {
          cur[k] = STBI__BYTECAST(raw[k] + (cur[k - output_bytes] >> 1));
        }
        break;
        STBI__CASE(STBI__F_paeth_first) {
          cur[k] =
              STBI__BYTECAST(raw[k] + stbi__paeth(cur[k - output_bytes], 0, 0));
        }
        break;
      }
#undef STBI__CASE

      // the loop above sets the high byte of the pixels' alpha, but for
      // 16 bit png files we also need the low byte set. we'll do that here.
      if (depth == 16) {
        cur = a->out + stride * j; // start at the beginning of the row again
        for (i = 0; i < x; ++i, cur += output_bytes) {
          cur[filter_bytes + 1] = 255;
        }
      }
    }
  }

  // we make a separate pass to expand bits to pixels; for performance,
  // this could run two scanlines behind the above code, so it won't
  // intefere with filtering but will still be in the cache.
  if (depth < 8) {
    for (j = 0; j < y; ++j) {
      stbi_uc *cur = a->out + stride * j;
      stbi_uc *in = a->out + stride * j + x * out_n - img_width_bytes;
      // unpack 1/2/4-bit into a 8-bit buffer. allows us to keep the common
      // 8-bit path optimal at minimal cost for 1/2/4-bit png guarante byte
      // alignment, if width is not multiple of 8/4/2 we'll decode dummy
      // trailing data that will be skipped in the later loop
      stbi_uc scale = (color == 0)
                          ? stbi__depth_scale_table[depth]
                          : 1; // scale grayscale values to 0..255 range

      // note that the final byte might overshoot and write more data than
      // desired. we can allocate enough data that this never writes out of
      // memory, but it could also overwrite the next scanline. can it overwrite
      // non-empty data on the next scanline? yes, consider 1-pixel-wide
      // scanlines with 1-bit-per-pixel. so we need to explicitly clamp the
      // final ones

      if (depth == 4) {
        for (k = x * img_n; k >= 2; k -= 2, ++in) {
          *cur++ = scale * ((*in >> 4));
          *cur++ = scale * ((*in) & 0x0f);
        }
        if (k > 0)
          *cur++ = scale * ((*in >> 4));
      } else if (depth == 2) {
        for (k = x * img_n; k >= 4; k -= 4, ++in) {
          *cur++ = scale * ((*in >> 6));
          *cur++ = scale * ((*in >> 4) & 0x03);
          *cur++ = scale * ((*in >> 2) & 0x03);
          *cur++ = scale * ((*in) & 0x03);
        }
        if (k > 0)
          *cur++ = scale * ((*in >> 6));
        if (k > 1)
          *cur++ = scale * ((*in >> 4) & 0x03);
        if (k > 2)
          *cur++ = scale * ((*in >> 2) & 0x03);
      } else if (depth == 1) {
        for (k = x * img_n; k >= 8; k -= 8, ++in) {
          *cur++ = scale * ((*in >> 7));
          *cur++ = scale * ((*in >> 6) & 0x01);
          *cur++ = scale * ((*in >> 5) & 0x01);
          *cur++ = scale * ((*in >> 4) & 0x01);
          *cur++ = scale * ((*in >> 3) & 0x01);
          *cur++ = scale * ((*in >> 2) & 0x01);
          *cur++ = scale * ((*in >> 1) & 0x01);
          *cur++ = scale * ((*in) & 0x01);
        }
        if (k > 0)
          *cur++ = scale * ((*in >> 7));
        if (k > 1)
          *cur++ = scale * ((*in >> 6) & 0x01);
        if (k > 2)
          *cur++ = scale * ((*in >> 5) & 0x01);
        if (k > 3)
          *cur++ = scale * ((*in >> 4) & 0x01);
        if (k > 4)
          *cur++ = scale * ((*in >> 3) & 0x01);
        if (k > 5)
          *cur++ = scale * ((*in >> 2) & 0x01);
        if (k > 6)
          *cur++ = scale * ((*in >> 1) & 0x01);
      }
      if (img_n != out_n) {
        int q;
        // insert alpha = 255
        cur = a->out + stride * j;
        if (img_n == 1) {
          for (q = x - 1; q >= 0; --q) {
            cur[q * 2 + 1] = 255;
            cur[q * 2 + 0] = cur[q];
          }
        } else {
          STBI_ASSERT(img_n == 3);
          for (q = x - 1; q >= 0; --q) {
            cur[q * 4 + 3] = 255;
            cur[q * 4 + 2] = cur[q * 3 + 2];
            cur[q * 4 + 1] = cur[q * 3 + 1];
            cur[q * 4 + 0] = cur[q * 3 + 0];
          }
        }
      }
    }
  } else if (depth == 16) {
    // force the image data from big-endian to platform-native.
    // this is done in a separate pass due to the decoding relying
    // on the data being untouched, but could probably be done
    // per-line during decode if care is taken.
    stbi_uc *cur = a->out;
    stbi__uint16 *cur16 = (stbi__uint16 *)cur;

    for (i = 0; i < x * y * out_n; ++i, cur16++, cur += 2) {
      *cur16 = (cur[0] << 8) | cur[1];
    }
  }

  return 1;
}

static int stbi__create_png_image(stbi__png *a, stbi_uc *image_data,
                                  stbi__uint32 image_data_len, int out_n,
                                  int depth, int color, int interlaced) {
  int bytes = (depth == 16 ? 2 : 1);
  int out_bytes = out_n * bytes;
  stbi_uc *final;
  int p;
  if (!interlaced)
    return stbi__create_png_image_raw(a, image_data, image_data_len, out_n,
                                      a->s->img_x, a->s->img_y, depth, color);

  // de-interlacing
  final = (stbi_uc *)stbi__malloc_mad3(a->s->img_x, a->s->img_y, out_bytes, 0);
  for (p = 0; p < 7; ++p) {
    int xorig[] = {0, 4, 0, 2, 0, 1, 0};
    int yorig[] = {0, 0, 4, 0, 2, 0, 1};
    int xspc[] = {8, 8, 4, 4, 2, 2, 1};
    int yspc[] = {8, 8, 8, 4, 4, 2, 2};
    int i, j, x, y;
    // pass1_x[4] = 0, pass1_x[5] = 1, pass1_x[12] = 1
    x = (a->s->img_x - xorig[p] + xspc[p] - 1) / xspc[p];
    y = (a->s->img_y - yorig[p] + yspc[p] - 1) / yspc[p];
    if (x && y) {
      stbi__uint32 img_len = ((((a->s->img_n * x * depth) + 7) >> 3) + 1) * y;
      if (!stbi__create_png_image_raw(a, image_data, image_data_len, out_n, x,
                                      y, depth, color)) {
        STBI_FREE(final);
        return 0;
      }
      for (j = 0; j < y; ++j) {
        for (i = 0; i < x; ++i) {
          int out_y = j * yspc[p] + yorig[p];
          int out_x = i * xspc[p] + xorig[p];
          memcpy(final + out_y * a->s->img_x * out_bytes + out_x * out_bytes,
                 a->out + (j * x + i) * out_bytes, out_bytes);
        }
      }
      STBI_FREE(a->out);
      image_data += img_len;
      image_data_len -= img_len;
    }
  }
  a->out = final;

  return 1;
}

static int stbi__compute_transparency(stbi__png *z, stbi_uc tc[3], int out_n) {
  stbi__context *s = z->s;
  stbi__uint32 i, pixel_count = s->img_x * s->img_y;
  stbi_uc *p = z->out;

  // compute color-based transparency, assuming we've
  // already got 255 as the alpha value in the output
  STBI_ASSERT(out_n == 2 || out_n == 4);

  if (out_n == 2) {
    for (i = 0; i < pixel_count; ++i) {
      p[1] = (p[0] == tc[0] ? 0 : 255);
      p += 2;
    }
  } else {
    for (i = 0; i < pixel_count; ++i) {
      if (p[0] == tc[0] && p[1] == tc[1] && p[2] == tc[2])
        p[3] = 0;
      p += 4;
    }
  }
  return 1;
}

static int stbi__compute_transparency16(stbi__png *z, stbi__uint16 tc[3],
                                        int out_n) {
  stbi__context *s = z->s;
  stbi__uint32 i, pixel_count = s->img_x * s->img_y;
  stbi__uint16 *p = (stbi__uint16 *)z->out;

  // compute color-based transparency, assuming we've
  // already got 65535 as the alpha value in the output
  STBI_ASSERT(out_n == 2 || out_n == 4);

  if (out_n == 2) {
    for (i = 0; i < pixel_count; ++i) {
      p[1] = (p[0] == tc[0] ? 0 : 65535);
      p += 2;
    }
  } else {
    for (i = 0; i < pixel_count; ++i) {
      if (p[0] == tc[0] && p[1] == tc[1] && p[2] == tc[2])
        p[3] = 0;
      p += 4;
    }
  }
  return 1;
}

static int stbi__expand_png_palette(stbi__png *a, stbi_uc *palette, int len,
                                    int pal_img_n) {
  stbi__uint32 i, pixel_count = a->s->img_x * a->s->img_y;
  stbi_uc *p, *temp_out, *orig = a->out;

  p = (stbi_uc *)stbi__malloc_mad2(pixel_count, pal_img_n, 0);
  if (p == NULL)
    return stbi__err("outofmem", "Out of memory");

  // between here and free(out) below, exitting would leak
  temp_out = p;

  if (pal_img_n == 3) {
    for (i = 0; i < pixel_count; ++i) {
      int n = orig[i] * 4;
      p[0] = palette[n];
      p[1] = palette[n + 1];
      p[2] = palette[n + 2];
      p += 3;
    }
  } else {
    for (i = 0; i < pixel_count; ++i) {
      int n = orig[i] * 4;
      p[0] = palette[n];
      p[1] = palette[n + 1];
      p[2] = palette[n + 2];
      p[3] = palette[n + 3];
      p += 4;
    }
  }
  STBI_FREE(a->out);
  a->out = temp_out;

  STBI_NOTUSED(len);

  return 1;
}

static int stbi__unpremultiply_on_load = 0;
static int stbi__de_iphone_flag = 0;

STBIDEF void
stbi_set_unpremultiply_on_load(int flag_true_if_should_unpremultiply) {
  stbi__unpremultiply_on_load = flag_true_if_should_unpremultiply;
}

STBIDEF void stbi_convert_iphone_png_to_rgb(int flag_true_if_should_convert) {
  stbi__de_iphone_flag = flag_true_if_should_convert;
}

static void stbi__de_iphone(stbi__png *z) {
  stbi__context *s = z->s;
  stbi__uint32 i, pixel_count = s->img_x * s->img_y;
  stbi_uc *p = z->out;

  if (s->img_out_n == 3) { // convert bgr to rgb
    for (i = 0; i < pixel_count; ++i) {
      stbi_uc t = p[0];
      p[0] = p[2];
      p[2] = t;
      p += 3;
    }
  } else {
    STBI_ASSERT(s->img_out_n == 4);
    if (stbi__unpremultiply_on_load) {
      // convert bgr to rgb and unpremultiply
      for (i = 0; i < pixel_count; ++i) {
        stbi_uc a = p[3];
        stbi_uc t = p[0];
        if (a) {
          stbi_uc half = a / 2;
          p[0] = (p[2] * 255 + half) / a;
          p[1] = (p[1] * 255 + half) / a;
          p[2] = (t * 255 + half) / a;
        } else {
          p[0] = p[2];
          p[2] = t;
        }
        p += 4;
      }
    } else {
      // convert bgr to rgb
      for (i = 0; i < pixel_count; ++i) {
        stbi_uc t = p[0];
        p[0] = p[2];
        p[2] = t;
        p += 4;
      }
    }
  }
}

#define STBI__PNG_TYPE(a, b, c, d)                                             \
  (((a) << 24) + ((b) << 16) + ((c) << 8) + (d))

static int stbi__parse_png_file(stbi__png *z, int scan, int req_comp) {
  stbi_uc palette[1024], pal_img_n = 0;
  stbi_uc has_trans = 0, tc[3];
  stbi__uint16 tc16[3];
  stbi__uint32 ioff = 0, idata_limit = 0, i, pal_len = 0;
  int first = 1, k, interlace = 0, color = 0, is_iphone = 0;
  stbi__context *s = z->s;

  z->expanded = NULL;
  z->idata = NULL;
  z->out = NULL;

  if (!stbi__check_png_header(s))
    return 0;

  if (scan == STBI__SCAN_type)
    return 1;

  for (;;) {
    stbi__pngchunk c = stbi__get_chunk_header(s);
    switch (c.type) {
    case STBI__PNG_TYPE('C', 'g', 'B', 'I'):
      is_iphone = 1;
      stbi__skip(s, c.length);
      break;
    case STBI__PNG_TYPE('I', 'H', 'D', 'R'): {
      int comp, filter;
      if (!first)
        return stbi__err("multiple IHDR", "Corrupt PNG");
      first = 0;
      if (c.length != 13)
        return stbi__err("bad IHDR len", "Corrupt PNG");
      s->img_x = stbi__get32be(s);
      if (s->img_x > (1 << 24))
        return stbi__err("too large", "Very large image (corrupt?)");
      s->img_y = stbi__get32be(s);
      if (s->img_y > (1 << 24))
        return stbi__err("too large", "Very large image (corrupt?)");
      z->depth = stbi__get8(s);
      if (z->depth != 1 && z->depth != 2 && z->depth != 4 && z->depth != 8 &&
          z->depth != 16)
        return stbi__err("1/2/4/8/16-bit only",
                         "PNG not supported: 1/2/4/8/16-bit only");
      color = stbi__get8(s);
      if (color > 6)
        return stbi__err("bad ctype", "Corrupt PNG");
      if (color == 3 && z->depth == 16)
        return stbi__err("bad ctype", "Corrupt PNG");
      if (color == 3)
        pal_img_n = 3;
      else if (color & 1)
        return stbi__err("bad ctype", "Corrupt PNG");
      comp = stbi__get8(s);
      if (comp)
        return stbi__err("bad comp method", "Corrupt PNG");
      filter = stbi__get8(s);
      if (filter)
        return stbi__err("bad filter method", "Corrupt PNG");
      interlace = stbi__get8(s);
      if (interlace > 1)
        return stbi__err("bad interlace method", "Corrupt PNG");
      if (!s->img_x || !s->img_y)
        return stbi__err("0-pixel image", "Corrupt PNG");
      if (!pal_img_n) {
        s->img_n = (color & 2 ? 3 : 1) + (color & 4 ? 1 : 0);
        if ((1 << 30) / s->img_x / s->img_n < s->img_y)
          return stbi__err("too large", "Image too large to decode");
        if (scan == STBI__SCAN_header)
          return 1;
      } else {
        // if paletted, then pal_n is our final components, and
        // img_n is # components to decompress/filter.
        s->img_n = 1;
        if ((1 << 30) / s->img_x / 4 < s->img_y)
          return stbi__err("too large", "Corrupt PNG");
        // if SCAN_header, have to scan to see if we have a tRNS
      }
      break;
    }

    case STBI__PNG_TYPE('P', 'L', 'T', 'E'): {
      if (first)
        return stbi__err("first not IHDR", "Corrupt PNG");
      if (c.length > 256 * 3)
        return stbi__err("invalid PLTE", "Corrupt PNG");
      pal_len = c.length / 3;
      if (pal_len * 3 != c.length)
        return stbi__err("invalid PLTE", "Corrupt PNG");
      for (i = 0; i < pal_len; ++i) {
        palette[i * 4 + 0] = stbi__get8(s);
        palette[i * 4 + 1] = stbi__get8(s);
        palette[i * 4 + 2] = stbi__get8(s);
        palette[i * 4 + 3] = 255;
      }
      break;
    }

    case STBI__PNG_TYPE('t', 'R', 'N', 'S'): {
      if (first)
        return stbi__err("first not IHDR", "Corrupt PNG");
      if (z->idata)
        return stbi__err("tRNS after IDAT", "Corrupt PNG");
      if (pal_img_n) {
        if (scan == STBI__SCAN_header) {
          s->img_n = 4;
          return 1;
        }
        if (pal_len == 0)
          return stbi__err("tRNS before PLTE", "Corrupt PNG");
        if (c.length > pal_len)
          return stbi__err("bad tRNS len", "Corrupt PNG");
        pal_img_n = 4;
        for (i = 0; i < c.length; ++i)
          palette[i * 4 + 3] = stbi__get8(s);
      } else {
        if (!(s->img_n & 1))
          return stbi__err("tRNS with alpha", "Corrupt PNG");
        if (c.length != (stbi__uint32)s->img_n * 2)
          return stbi__err("bad tRNS len", "Corrupt PNG");
        has_trans = 1;
        if (z->depth == 16) {
          for (k = 0; k < s->img_n; ++k)
            tc16[k] = (stbi__uint16)stbi__get16be(s); // copy the values as-is
        } else {
          for (k = 0; k < s->img_n; ++k)
            tc[k] = (stbi_uc)(stbi__get16be(s) & 255) *
                    stbi__depth_scale_table[z->depth]; // non 8-bit images will
                                                       // be larger
        }
      }
      break;
    }

    case STBI__PNG_TYPE('I', 'D', 'A', 'T'): {
      if (first)
        return stbi__err("first not IHDR", "Corrupt PNG");
      if (pal_img_n && !pal_len)
        return stbi__err("no PLTE", "Corrupt PNG");
      if (scan == STBI__SCAN_header) {
        s->img_n = pal_img_n;
        return 1;
      }
      if ((int)(ioff + c.length) < (int)ioff)
        return 0;
      if (ioff + c.length > idata_limit) {
        stbi__uint32 idata_limit_old = idata_limit;
        stbi_uc *p;
        if (idata_limit == 0)
          idata_limit = c.length > 4096 ? c.length : 4096;
        while (ioff + c.length > idata_limit)
          idata_limit *= 2;
        STBI_NOTUSED(idata_limit_old);
        p = (stbi_uc *)STBI_REALLOC_SIZED(z->idata, idata_limit_old,
                                          idata_limit);
        if (p == NULL)
          return stbi__err("outofmem", "Out of memory");
        z->idata = p;
      }
      if (!stbi__getn(s, z->idata + ioff, c.length))
        return stbi__err("outofdata", "Corrupt PNG");
      ioff += c.length;
      break;
    }

    case STBI__PNG_TYPE('I', 'E', 'N', 'D'): {
      stbi__uint32 raw_len, bpl;
      if (first)
        return stbi__err("first not IHDR", "Corrupt PNG");
      if (scan != STBI__SCAN_load)
        return 1;
      if (z->idata == NULL)
        return stbi__err("no IDAT", "Corrupt PNG");
      // initial guess for decoded data size to avoid unnecessary reallocs
      bpl = (s->img_x * z->depth + 7) / 8; // bytes per line, per component
      raw_len = bpl * s->img_y * s->img_n /* pixels */ +
                s->img_y /* filter mode per row */;
      z->expanded = (stbi_uc *)stbi_zlib_decode_malloc_guesssize_headerflag(
          (char *)z->idata, ioff, raw_len, (int *)&raw_len, !is_iphone);
      if (z->expanded == NULL)
        return 0; // zlib should set error
      STBI_FREE(z->idata);
      z->idata = NULL;
      if ((req_comp == s->img_n + 1 && req_comp != 3 && !pal_img_n) ||
          has_trans)
        s->img_out_n = s->img_n + 1;
      else
        s->img_out_n = s->img_n;
      if (!stbi__create_png_image(z, z->expanded, raw_len, s->img_out_n,
                                  z->depth, color, interlace))
        return 0;
      if (has_trans) {
        if (z->depth == 16) {
          if (!stbi__compute_transparency16(z, tc16, s->img_out_n))
            return 0;
        } else {
          if (!stbi__compute_transparency(z, tc, s->img_out_n))
            return 0;
        }
      }
      if (is_iphone && stbi__de_iphone_flag && s->img_out_n > 2)
        stbi__de_iphone(z);
      if (pal_img_n) {
        // pal_img_n == 3 or 4
        s->img_n = pal_img_n; // record the actual colors we had
        s->img_out_n = pal_img_n;
        if (req_comp >= 3)
          s->img_out_n = req_comp;
        if (!stbi__expand_png_palette(z, palette, pal_len, s->img_out_n))
          return 0;
      } else if (has_trans) {
        // non-paletted image with tRNS -> source image has (constant) alpha
        ++s->img_n;
      }
      STBI_FREE(z->expanded);
      z->expanded = NULL;
      return 1;
    }

    default:
      // if critical, fail
      if (first)
        return stbi__err("first not IHDR", "Corrupt PNG");
      if ((c.type & (1 << 29)) == 0) {
#ifndef STBI_NO_FAILURE_STRINGS
        // not threadsafe
        static char invalid_chunk[] = "XXXX PNG chunk not known";
        invalid_chunk[0] = STBI__BYTECAST(c.type >> 24);
        invalid_chunk[1] = STBI__BYTECAST(c.type >> 16);
        invalid_chunk[2] = STBI__BYTECAST(c.type >> 8);
        invalid_chunk[3] = STBI__BYTECAST(c.type >> 0);
#endif
        return stbi__err(invalid_chunk,
                         "PNG not supported: unknown PNG chunk type");
      }
      stbi__skip(s, c.length);
      break;
    }
    // end of PNG chunk, read and skip CRC
    stbi__get32be(s);
  }
}

static void *stbi__do_png(stbi__png *p, int *x, int *y, int *n, int req_comp,
                          stbi__result_info *ri) {
  void *result = NULL;
  if (req_comp < 0 || req_comp > 4)
    return stbi__errpuc("bad req_comp", "Internal error");
  if (stbi__parse_png_file(p, STBI__SCAN_load, req_comp)) {
    if (p->depth < 8)
      ri->bits_per_channel = 8;
    else
      ri->bits_per_channel = p->depth;
    result = p->out;
    p->out = NULL;
    if (req_comp && req_comp != p->s->img_out_n) {
      if (ri->bits_per_channel == 8)
        result = stbi__convert_format((unsigned char *)result, p->s->img_out_n,
                                      req_comp, p->s->img_x, p->s->img_y);
      else
        result = stbi__convert_format16((stbi__uint16 *)result, p->s->img_out_n,
                                        req_comp, p->s->img_x, p->s->img_y);
      p->s->img_out_n = req_comp;
      if (result == NULL)
        return result;
    }
    *x = p->s->img_x;
    *y = p->s->img_y;
    if (n)
      *n = p->s->img_n;
  }
  STBI_FREE(p->out);
  p->out = NULL;
  STBI_FREE(p->expanded);
  p->expanded = NULL;
  STBI_FREE(p->idata);
  p->idata = NULL;

  return result;
}

static void *stbi__png_load(stbi__context *s, int *x, int *y, int *comp,
                            int req_comp, stbi__result_info *ri) {
  stbi__png p;
  p.s = s;
  return stbi__do_png(&p, x, y, comp, req_comp, ri);
}

static int stbi__png_test(stbi__context *s) {
  int r;
  r = stbi__check_png_header(s);
  stbi__rewind(s);
  return r;
}

static int stbi__png_info_raw(stbi__png *p, int *x, int *y, int *comp) {
  if (!stbi__parse_png_file(p, STBI__SCAN_header, 0)) {
    stbi__rewind(p->s);
    return 0;
  }
  if (x)
    *x = p->s->img_x;
  if (y)
    *y = p->s->img_y;
  if (comp)
    *comp = p->s->img_n;
  return 1;
}

static int stbi__png_info(stbi__context *s, int *x, int *y, int *comp) {
  stbi__png p;
  p.s = s;
  return stbi__png_info_raw(&p, x, y, comp);
}
#endif

  // Microsoft/Windows BMP image

#ifndef STBI_NO_BMP
static int stbi__bmp_test_raw(stbi__context *s) {
  int r;
  int sz;
  if (stbi__get8(s) != 'B')
    return 0;
  if (stbi__get8(s) != 'M')
    return 0;
  stbi__get32le(s); // discard filesize
  stbi__get16le(s); // discard reserved
  stbi__get16le(s); // discard reserved
  stbi__get32le(s); // discard data offset
  sz = stbi__get32le(s);
  r = (sz == 12 || sz == 40 || sz == 56 || sz == 108 || sz == 124);
  return r;
}

static int stbi__bmp_test(stbi__context *s) {
  int r = stbi__bmp_test_raw(s);
  stbi__rewind(s);
  return r;
}

// returns 0..31 for the highest set bit
static int stbi__high_bit(unsigned int z) {
  int n = 0;
  if (z == 0)
    return -1;
  if (z >= 0x10000)
    n += 16, z >>= 16;
  if (z >= 0x00100)
    n += 8, z >>= 8;
  if (z >= 0x00010)
    n += 4, z >>= 4;
  if (z >= 0x00004)
    n += 2, z >>= 2;
  if (z >= 0x00002)
    n += 1, z >>= 1;
  return n;
}

static int stbi__bitcount(unsigned int a) {
  a = (a & 0x55555555) + ((a >> 1) & 0x55555555); // max 2
  a = (a & 0x33333333) + ((a >> 2) & 0x33333333); // max 4
  a = (a + (a >> 4)) & 0x0f0f0f0f;                // max 8 per 4, now 8 bits
  a = (a + (a >> 8));                             // max 16 per 8 bits
  a = (a + (a >> 16));                            // max 32 per 8 bits
  return a & 0xff;
}

static int stbi__shiftsigned(int v, int shift, int bits) {
  int result;
  int z = 0;

  if (shift < 0)
    v <<= -shift;
  else
    v >>= shift;
  result = v;

  z = bits;
  while (z < 8) {
    result += v >> z;
    z += bits;
  }
  return result;
}

typedef struct {
  int bpp, offset, hsz;
  unsigned int mr, mg, mb, ma, all_a;
} stbi__bmp_data;

static void *stbi__bmp_parse_header(stbi__context *s, stbi__bmp_data *info) {
  int hsz;
  if (stbi__get8(s) != 'B' || stbi__get8(s) != 'M')
    return stbi__errpuc("not BMP", "Corrupt BMP");
  stbi__get32le(s); // discard filesize
  stbi__get16le(s); // discard reserved
  stbi__get16le(s); // discard reserved
  info->offset = stbi__get32le(s);
  info->hsz = hsz = stbi__get32le(s);
  info->mr = info->mg = info->mb = info->ma = 0;

  if (hsz != 12 && hsz != 40 && hsz != 56 && hsz != 108 && hsz != 124)
    return stbi__errpuc("unknown BMP", "BMP type not supported: unknown");
  if (hsz == 12) {
    s->img_x = stbi__get16le(s);
    s->img_y = stbi__get16le(s);
  } else {
    s->img_x = stbi__get32le(s);
    s->img_y = stbi__get32le(s);
  }
  if (stbi__get16le(s) != 1)
    return stbi__errpuc("bad BMP", "bad BMP");
  info->bpp = stbi__get16le(s);
  if (info->bpp == 1)
    return stbi__errpuc("monochrome", "BMP type not supported: 1-bit");
  if (hsz != 12) {
    int compress = stbi__get32le(s);
    if (compress == 1 || compress == 2)
      return stbi__errpuc("BMP RLE", "BMP type not supported: RLE");
    stbi__get32le(s); // discard sizeof
    stbi__get32le(s); // discard hres
    stbi__get32le(s); // discard vres
    stbi__get32le(s); // discard colorsused
    stbi__get32le(s); // discard max important
    if (hsz == 40 || hsz == 56) {
      if (hsz == 56) {
        stbi__get32le(s);
        stbi__get32le(s);
        stbi__get32le(s);
        stbi__get32le(s);
      }
      if (info->bpp == 16 || info->bpp == 32) {
        if (compress == 0) {
          if (info->bpp == 32) {
            info->mr = 0xffu << 16;
            info->mg = 0xffu << 8;
            info->mb = 0xffu << 0;
            info->ma = 0xffu << 24;
            info->all_a = 0; // if all_a is 0 at end, then we loaded alpha
                             // channel but it was all 0
          } else {
            info->mr = 31u << 10;
            info->mg = 31u << 5;
            info->mb = 31u << 0;
          }
        } else if (compress == 3) {
          info->mr = stbi__get32le(s);
          info->mg = stbi__get32le(s);
          info->mb = stbi__get32le(s);
          // not documented, but generated by photoshop and handled by mspaint
          if (info->mr == info->mg && info->mg == info->mb) {
            // ?!?!?
            return stbi__errpuc("bad BMP", "bad BMP");
          }
        } else
          return stbi__errpuc("bad BMP", "bad BMP");
      }
    } else {
      int i;
      if (hsz != 108 && hsz != 124)
        return stbi__errpuc("bad BMP", "bad BMP");
      info->mr = stbi__get32le(s);
      info->mg = stbi__get32le(s);
      info->mb = stbi__get32le(s);
      info->ma = stbi__get32le(s);
      stbi__get32le(s); // discard color space
      for (i = 0; i < 12; ++i)
        stbi__get32le(s); // discard color space parameters
      if (hsz == 124) {
        stbi__get32le(s); // discard rendering intent
        stbi__get32le(s); // discard offset of profile data
        stbi__get32le(s); // discard size of profile data
        stbi__get32le(s); // discard reserved
      }
    }
  }
  return (void *)1;
}

static void *stbi__bmp_load(stbi__context *s, int *x, int *y, int *comp,
                            int req_comp, stbi__result_info *ri) {
  stbi_uc *out;
  unsigned int mr = 0, mg = 0, mb = 0, ma = 0, all_a;
  stbi_uc pal[256][4];
  int psize = 0, i, j, width;
  int flip_vertically, pad, target;
  stbi__bmp_data info;
  STBI_NOTUSED(ri);

  info.all_a = 255;
  if (stbi__bmp_parse_header(s, &info) == NULL)
    return NULL; // error code already set

  flip_vertically = ((int)s->img_y) > 0;
  s->img_y = abs((int)s->img_y);

  mr = info.mr;
  mg = info.mg;
  mb = info.mb;
  ma = info.ma;
  all_a = info.all_a;

  if (info.hsz == 12) {
    if (info.bpp < 24)
      psize = (info.offset - 14 - 24) / 3;
  } else {
    if (info.bpp < 16)
      psize = (info.offset - 14 - info.hsz) >> 2;
  }

  s->img_n = ma ? 4 : 3;
  if (req_comp && req_comp >= 3) // we can directly decode 3 or 4
    target = req_comp;
  else
    target = s->img_n; // if they want monochrome, we'll post-convert

  // sanity-check size
  if (!stbi__mad3sizes_valid(target, s->img_x, s->img_y, 0))
    return stbi__errpuc("too large", "Corrupt BMP");

  out = (stbi_uc *)stbi__malloc_mad3(target, s->img_x, s->img_y, 0);
  if (!out)
    return stbi__errpuc("outofmem", "Out of memory");
  if (info.bpp < 16) {
    int z = 0;
    if (psize == 0 || psize > 256) {
      STBI_FREE(out);
      return stbi__errpuc("invalid", "Corrupt BMP");
    }
    for (i = 0; i < psize; ++i) {
      pal[i][2] = stbi__get8(s);
      pal[i][1] = stbi__get8(s);
      pal[i][0] = stbi__get8(s);
      if (info.hsz != 12)
        stbi__get8(s);
      pal[i][3] = 255;
    }
    stbi__skip(s,
               info.offset - 14 - info.hsz - psize * (info.hsz == 12 ? 3 : 4));
    if (info.bpp == 4)
      width = (s->img_x + 1) >> 1;
    else if (info.bpp == 8)
      width = s->img_x;
    else {
      STBI_FREE(out);
      return stbi__errpuc("bad bpp", "Corrupt BMP");
    }
    pad = (-width) & 3;
    for (j = 0; j < (int)s->img_y; ++j) {
      for (i = 0; i < (int)s->img_x; i += 2) {
        int v = stbi__get8(s), v2 = 0;
        if (info.bpp == 4) {
          v2 = v & 15;
          v >>= 4;
        }
        out[z++] = pal[v][0];
        out[z++] = pal[v][1];
        out[z++] = pal[v][2];
        if (target == 4)
          out[z++] = 255;
        if (i + 1 == (int)s->img_x)
          break;
        v = (info.bpp == 8) ? stbi__get8(s) : v2;
        out[z++] = pal[v][0];
        out[z++] = pal[v][1];
        out[z++] = pal[v][2];
        if (target == 4)
          out[z++] = 255;
      }
      stbi__skip(s, pad);
    }
  } else {
    int rshift = 0, gshift = 0, bshift = 0, ashift = 0, rcount = 0, gcount = 0,
        bcount = 0, acount = 0;
    int z = 0;
    int easy = 0;
    stbi__skip(s, info.offset - 14 - info.hsz);
    if (info.bpp == 24)
      width = 3 * s->img_x;
    else if (info.bpp == 16)
      width = 2 * s->img_x;
    else /* bpp = 32 and pad = 0 */
      width = 0;
    pad = (-width) & 3;
    if (info.bpp == 24) {
      easy = 1;
    } else if (info.bpp == 32) {
      if (mb == 0xff && mg == 0xff00 && mr == 0x00ff0000 && ma == 0xff000000)
        easy = 2;
    }
    if (!easy) {
      if (!mr || !mg || !mb) {
        STBI_FREE(out);
        return stbi__errpuc("bad masks", "Corrupt BMP");
      }
      // right shift amt to put high bit in position #7
      rshift = stbi__high_bit(mr) - 7;
      rcount = stbi__bitcount(mr);
      gshift = stbi__high_bit(mg) - 7;
      gcount = stbi__bitcount(mg);
      bshift = stbi__high_bit(mb) - 7;
      bcount = stbi__bitcount(mb);
      ashift = stbi__high_bit(ma) - 7;
      acount = stbi__bitcount(ma);
    }
    for (j = 0; j < (int)s->img_y; ++j) {
      if (easy) {
        for (i = 0; i < (int)s->img_x; ++i) {
          unsigned char a;
          out[z + 2] = stbi__get8(s);
          out[z + 1] = stbi__get8(s);
          out[z + 0] = stbi__get8(s);
          z += 3;
          a = (easy == 2 ? stbi__get8(s) : 255);
          all_a |= a;
          if (target == 4)
            out[z++] = a;
        }
      } else {
        int bpp = info.bpp;
        for (i = 0; i < (int)s->img_x; ++i) {
          stbi__uint32 v =
              (bpp == 16 ? (stbi__uint32)stbi__get16le(s) : stbi__get32le(s));
          int a;
          out[z++] = STBI__BYTECAST(stbi__shiftsigned(v & mr, rshift, rcount));
          out[z++] = STBI__BYTECAST(stbi__shiftsigned(v & mg, gshift, gcount));
          out[z++] = STBI__BYTECAST(stbi__shiftsigned(v & mb, bshift, bcount));
          a = (ma ? stbi__shiftsigned(v & ma, ashift, acount) : 255);
          all_a |= a;
          if (target == 4)
            out[z++] = STBI__BYTECAST(a);
        }
      }
      stbi__skip(s, pad);
    }
  }

  // if alpha channel is all 0s, replace with all 255s
  if (target == 4 && all_a == 0)
    for (i = 4 * s->img_x * s->img_y - 1; i >= 0; i -= 4)
      out[i] = 255;

  if (flip_vertically) {
    stbi_uc t;
    for (j = 0; j<(int)s->img_y>> 1; ++j) {
      stbi_uc *p1 = out + j * s->img_x * target;
      stbi_uc *p2 = out + (s->img_y - 1 - j) * s->img_x * target;
      for (i = 0; i < (int)s->img_x * target; ++i) {
        t = p1[i], p1[i] = p2[i], p2[i] = t;
      }
    }
  }

  if (req_comp && req_comp != target) {
    out = stbi__convert_format(out, target, req_comp, s->img_x, s->img_y);
    if (out == NULL)
      return out; // stbi__convert_format frees input on failure
  }

  *x = s->img_x;
  *y = s->img_y;
  if (comp)
    *comp = s->img_n;
  return out;
}
#endif

// Targa Truevision - TGA
// by Jonathan Dummer
#ifndef STBI_NO_TGA
// returns STBI_rgb or whatever, 0 on error
static int stbi__tga_get_comp(int bits_per_pixel, int is_grey, int *is_rgb16) {
  // only RGB or RGBA (incl. 16bit) or grey allowed
  if (is_rgb16)
    *is_rgb16 = 0;
  switch (bits_per_pixel) {
  case 8:
    return STBI_grey;
  case 16:
    if (is_grey)
      return STBI_grey_alpha;
    else if (is_rgb16)
      *is_rgb16 = 1;
    return STBI_rgb; // GIVR: Edited this to silence fallthrough warning.
  case 15:
    if (is_rgb16)
      *is_rgb16 = 1;
    return STBI_rgb;
  case 24:
    return bits_per_pixel /
           8; // GIVR: Edited this to silence fallthrough warning.
  case 32:
    return bits_per_pixel / 8;
  default:
    return 0;
  }
}

static int stbi__tga_info(stbi__context *s, int *x, int *y, int *comp) {
  int tga_w, tga_h, tga_comp, tga_image_type, tga_bits_per_pixel,
      tga_colormap_bpp;
  int sz, tga_colormap_type;
  stbi__get8(s);                     // discard Offset
  tga_colormap_type = stbi__get8(s); // colormap type
  if (tga_colormap_type > 1) {
    stbi__rewind(s);
    return 0; // only RGB or indexed allowed
  }
  tga_image_type = stbi__get8(s); // image type
  if (tga_colormap_type == 1) {   // colormapped (paletted) image
    if (tga_image_type != 1 && tga_image_type != 9) {
      stbi__rewind(s);
      return 0;
    }
    stbi__skip(s,
               4); // skip index of first colormap entry and number of entries
    sz = stbi__get8(s); //   check bits per palette color entry
    if ((sz != 8) && (sz != 15) && (sz != 16) && (sz != 24) && (sz != 32)) {
      stbi__rewind(s);
      return 0;
    }
    stbi__skip(s, 4); // skip image x and y origin
    tga_colormap_bpp = sz;
  } else { // "normal" image w/o colormap - only RGB or grey allowed, +/- RLE
    if ((tga_image_type != 2) && (tga_image_type != 3) &&
        (tga_image_type != 10) && (tga_image_type != 11)) {
      stbi__rewind(s);
      return 0; // only RGB or grey allowed, +/- RLE
    }
    stbi__skip(s, 9); // skip colormap specification and image x/y origin
    tga_colormap_bpp = 0;
  }
  tga_w = stbi__get16le(s);
  if (tga_w < 1) {
    stbi__rewind(s);
    return 0; // test width
  }
  tga_h = stbi__get16le(s);
  if (tga_h < 1) {
    stbi__rewind(s);
    return 0; // test height
  }
  tga_bits_per_pixel = stbi__get8(s); // bits per pixel
  stbi__get8(s);                      // ignore alpha bits
  if (tga_colormap_bpp != 0) {
    if ((tga_bits_per_pixel != 8) && (tga_bits_per_pixel != 16)) {
      // when using a colormap, tga_bits_per_pixel is the size of the indexes
      // I don't think anything but 8 or 16bit indexes makes sense
      stbi__rewind(s);
      return 0;
    }
    tga_comp = stbi__tga_get_comp(tga_colormap_bpp, 0, NULL);
  } else {
    tga_comp = stbi__tga_get_comp(
        tga_bits_per_pixel, (tga_image_type == 3) || (tga_image_type == 11),
        NULL);
  }
  if (!tga_comp) {
    stbi__rewind(s);
    return 0;
  }
  if (x)
    *x = tga_w;
  if (y)
    *y = tga_h;
  if (comp)
    *comp = tga_comp;
  return 1; // seems to have passed everything
}

static int stbi__tga_test(stbi__context *s) {
  int res = 0;
  int sz, tga_color_type;
  stbi__get8(s);                  //   discard Offset
  tga_color_type = stbi__get8(s); //   color type
  if (tga_color_type > 1)
    goto errorEnd;           //   only RGB or indexed allowed
  sz = stbi__get8(s);        //   image type
  if (tga_color_type == 1) { // colormapped (paletted) image
    if (sz != 1 && sz != 9)
      goto errorEnd; // colortype 1 demands image type 1 or 9
    stbi__skip(s,
               4); // skip index of first colormap entry and number of entries
    sz = stbi__get8(s); //   check bits per palette color entry
    if ((sz != 8) && (sz != 15) && (sz != 16) && (sz != 24) && (sz != 32))
      goto errorEnd;
    stbi__skip(s, 4); // skip image x and y origin
  } else {            // "normal" image w/o colormap
    if ((sz != 2) && (sz != 3) && (sz != 10) && (sz != 11))
      goto errorEnd;  // only RGB or grey allowed, +/- RLE
    stbi__skip(s, 9); // skip colormap specification and image x/y origin
  }
  if (stbi__get16le(s) < 1)
    goto errorEnd; //   test width
  if (stbi__get16le(s) < 1)
    goto errorEnd;    //   test height
  sz = stbi__get8(s); //   bits per pixel
  if ((tga_color_type == 1) && (sz != 8) && (sz != 16))
    goto errorEnd; // for colormapped images, bpp is size of an index
  if ((sz != 8) && (sz != 15) && (sz != 16) && (sz != 24) && (sz != 32))
    goto errorEnd;

  res = 1; // if we got this far, everything's good and we can return 1 instead
           // of 0

errorEnd:
  stbi__rewind(s);
  return res;
}

// read 16bit value and convert to 24bit RGB
static void stbi__tga_read_rgb16(stbi__context *s, stbi_uc *out) {
  stbi__uint16 px = (stbi__uint16)stbi__get16le(s);
  stbi__uint16 fiveBitMask = 31;
  // we have 3 channels with 5bits each
  int r = (px >> 10) & fiveBitMask;
  int g = (px >> 5) & fiveBitMask;
  int b = px & fiveBitMask;
  // Note that this saves the data in RGB(A) order, so it doesn't need to be
  // swapped later
  out[0] = (stbi_uc)((r * 255) / 31);
  out[1] = (stbi_uc)((g * 255) / 31);
  out[2] = (stbi_uc)((b * 255) / 31);

  // some people claim that the most significant bit might be used for alpha
  // (possibly if an alpha-bit is set in the "image descriptor byte")
  // but that only made 16bit test images completely translucent..
  // so let's treat all 15 and 16bit TGAs as RGB with no alpha.
}

static void *stbi__tga_load(stbi__context *s, int *x, int *y, int *comp,
                            int req_comp, stbi__result_info *ri) {
  //   read in the TGA header stuff
  int tga_offset = stbi__get8(s);
  int tga_indexed = stbi__get8(s);
  int tga_image_type = stbi__get8(s);
  int tga_is_RLE = 0;
  int tga_palette_start = stbi__get16le(s);
  int tga_palette_len = stbi__get16le(s);
  int tga_palette_bits = stbi__get8(s);
  int tga_x_origin = stbi__get16le(s);
  int tga_y_origin = stbi__get16le(s);
  int tga_width = stbi__get16le(s);
  int tga_height = stbi__get16le(s);
  int tga_bits_per_pixel = stbi__get8(s);
  int tga_comp, tga_rgb16 = 0;
  int tga_inverted = stbi__get8(s);
  // int tga_alpha_bits = tga_inverted & 15; // the 4 lowest bits - unused
  // (useless?)
  //   image data
  unsigned char *tga_data;
  unsigned char *tga_palette = NULL;
  int i, j;
  unsigned char raw_data[4] = {0};
  int RLE_count = 0;
  int RLE_repeating = 0;
  int read_next_pixel = 1;
  STBI_NOTUSED(ri);

  //   do a tiny bit of precessing
  if (tga_image_type >= 8) {
    tga_image_type -= 8;
    tga_is_RLE = 1;
  }
  tga_inverted = 1 - ((tga_inverted >> 5) & 1);

  //   If I'm paletted, then I'll use the number of bits from the palette
  if (tga_indexed)
    tga_comp = stbi__tga_get_comp(tga_palette_bits, 0, &tga_rgb16);
  else
    tga_comp = stbi__tga_get_comp(tga_bits_per_pixel, (tga_image_type == 3),
                                  &tga_rgb16);

  if (!tga_comp) // shouldn't really happen, stbi__tga_test() should have
                 // ensured basic consistency
    return stbi__errpuc("bad format", "Can't find out TGA pixelformat");

  //   tga info
  *x = tga_width;
  *y = tga_height;
  if (comp)
    *comp = tga_comp;

  if (!stbi__mad3sizes_valid(tga_width, tga_height, tga_comp, 0))
    return stbi__errpuc("too large", "Corrupt TGA");

  tga_data =
      (unsigned char *)stbi__malloc_mad3(tga_width, tga_height, tga_comp, 0);
  if (!tga_data)
    return stbi__errpuc("outofmem", "Out of memory");

  // skip to the data's starting position (offset usually = 0)
  stbi__skip(s, tga_offset);

  if (!tga_indexed && !tga_is_RLE && !tga_rgb16) {
    for (i = 0; i < tga_height; ++i) {
      int row = tga_inverted ? tga_height - i - 1 : i;
      stbi_uc *tga_row = tga_data + row * tga_width * tga_comp;
      stbi__getn(s, tga_row, tga_width * tga_comp);
    }
  } else {
    //   do I need to load a palette?
    if (tga_indexed) {
      //   any data to skip? (offset usually = 0)
      stbi__skip(s, tga_palette_start);
      //   load the palette
      tga_palette =
          (unsigned char *)stbi__malloc_mad2(tga_palette_len, tga_comp, 0);
      if (!tga_palette) {
        STBI_FREE(tga_data);
        return stbi__errpuc("outofmem", "Out of memory");
      }
      if (tga_rgb16) {
        stbi_uc *pal_entry = tga_palette;
        STBI_ASSERT(tga_comp == STBI_rgb);
        for (i = 0; i < tga_palette_len; ++i) {
          stbi__tga_read_rgb16(s, pal_entry);
          pal_entry += tga_comp;
        }
      } else if (!stbi__getn(s, tga_palette, tga_palette_len * tga_comp)) {
        STBI_FREE(tga_data);
        STBI_FREE(tga_palette);
        return stbi__errpuc("bad palette", "Corrupt TGA");
      }
    }
    //   load the data
    for (i = 0; i < tga_width * tga_height; ++i) {
      //   if I'm in RLE mode, do I need to get a RLE stbi__pngchunk?
      if (tga_is_RLE) {
        if (RLE_count == 0) {
          //   yep, get the next byte as a RLE command
          int RLE_cmd = stbi__get8(s);
          RLE_count = 1 + (RLE_cmd & 127);
          RLE_repeating = RLE_cmd >> 7;
          read_next_pixel = 1;
        } else if (!RLE_repeating) {
          read_next_pixel = 1;
        }
      } else {
        read_next_pixel = 1;
      }
      //   OK, if I need to read a pixel, do it now
      if (read_next_pixel) {
        //   load however much data we did have
        if (tga_indexed) {
          // read in index, then perform the lookup
          int pal_idx =
              (tga_bits_per_pixel == 8) ? stbi__get8(s) : stbi__get16le(s);
          if (pal_idx >= tga_palette_len) {
            // invalid index
            pal_idx = 0;
          }
          pal_idx *= tga_comp;
          for (j = 0; j < tga_comp; ++j) {
            raw_data[j] = tga_palette[pal_idx + j];
          }
        } else if (tga_rgb16) {
          STBI_ASSERT(tga_comp == STBI_rgb);
          stbi__tga_read_rgb16(s, raw_data);
        } else {
          //   read in the data raw
          for (j = 0; j < tga_comp; ++j) {
            raw_data[j] = stbi__get8(s);
          }
        }
        //   clear the reading flag for the next pixel
        read_next_pixel = 0;
      } // end of reading a pixel

      // copy data
      for (j = 0; j < tga_comp; ++j)
        tga_data[i * tga_comp + j] = raw_data[j];

      //   in case we're in RLE mode, keep counting down
      --RLE_count;
    }
    //   do I need to invert the image?
    if (tga_inverted) {
      for (j = 0; j * 2 < tga_height; ++j) {
        int index1 = j * tga_width * tga_comp;
        int index2 = (tga_height - 1 - j) * tga_width * tga_comp;
        for (i = tga_width * tga_comp; i > 0; --i) {
          unsigned char temp = tga_data[index1];
          tga_data[index1] = tga_data[index2];
          tga_data[index2] = temp;
          ++index1;
          ++index2;
        }
      }
    }
    //   clear my palette, if I had one
    if (tga_palette != NULL) {
      STBI_FREE(tga_palette);
    }
  }

  // swap RGB - if the source data was RGB16, it already is in the right order
  if (tga_comp >= 3 && !tga_rgb16) {
    unsigned char *tga_pixel = tga_data;
    for (i = 0; i < tga_width * tga_height; ++i) {
      unsigned char temp = tga_pixel[0];
      tga_pixel[0] = tga_pixel[2];
      tga_pixel[2] = temp;
      tga_pixel += tga_comp;
    }
  }

  // convert to target component count
  if (req_comp && req_comp != tga_comp)
    tga_data = stbi__convert_format(tga_data, tga_comp, req_comp, tga_width,
                                    tga_height);

  //   the things I do to get rid of an error message, and yet keep
  //   Microsoft's C compilers happy... [8^(
  tga_palette_start = tga_palette_len = tga_palette_bits = tga_x_origin =
      tga_y_origin = 0;
  //   OK, done
  return tga_data;
}
#endif

  // *************************************************************************************************
  // Photoshop PSD loader -- PD by Thatcher Ulrich, integration by Nicolas
  // Schulz, tweaked by STB

#ifndef STBI_NO_PSD
static int stbi__psd_test(stbi__context *s) {
  int r = (stbi__get32be(s) == 0x38425053);
  stbi__rewind(s);
  return r;
}

static int stbi__psd_decode_rle(stbi__context *s, stbi_uc *p, int pixelCount) {
  int count, nleft, len;

  count = 0;
  while ((nleft = pixelCount - count) > 0) {
    len = stbi__get8(s);
    if (len == 128) {
      // No-op.
    } else if (len < 128) {
      // Copy next len+1 bytes literally.
      len++;
      if (len > nleft)
        return 0; // corrupt data
      count += len;
      while (len) {
        *p = stbi__get8(s);
        p += 4;
        len--;
      }
    } else if (len > 128) {
      stbi_uc val;
      // Next -len+1 bytes in the dest are replicated from next source byte.
      // (Interpret len as a negative 8-bit int.)
      len = 257 - len;
      if (len > nleft)
        return 0; // corrupt data
      val = stbi__get8(s);
      count += len;
      while (len) {
        *p = val;
        p += 4;
        len--;
      }
    }
  }

  return 1;
}

static void *stbi__psd_load(stbi__context *s, int *x, int *y, int *comp,
                            int req_comp, stbi__result_info *ri, int bpc) {
  int pixelCount;
  int channelCount, compression;
  int channel, i;
  int bitdepth;
  int w, h;
  stbi_uc *out;
  STBI_NOTUSED(ri);

  // Check identifier
  if (stbi__get32be(s) != 0x38425053) // "8BPS"
    return stbi__errpuc("not PSD", "Corrupt PSD image");

  // Check file type version.
  if (stbi__get16be(s) != 1)
    return stbi__errpuc("wrong version", "Unsupported version of PSD image");

  // Skip 6 reserved bytes.
  stbi__skip(s, 6);

  // Read the number of channels (R, G, B, A, etc).
  channelCount = stbi__get16be(s);
  if (channelCount < 0 || channelCount > 16)
    return stbi__errpuc("wrong channel count",
                        "Unsupported number of channels in PSD image");

  // Read the rows and columns of the image.
  h = stbi__get32be(s);
  w = stbi__get32be(s);

  // Make sure the depth is 8 bits.
  bitdepth = stbi__get16be(s);
  if (bitdepth != 8 && bitdepth != 16)
    return stbi__errpuc("unsupported bit depth",
                        "PSD bit depth is not 8 or 16 bit");

  // Make sure the color mode is RGB.
  // Valid options are:
  //   0: Bitmap
  //   1: Grayscale
  //   2: Indexed color
  //   3: RGB color
  //   4: CMYK color
  //   7: Multichannel
  //   8: Duotone
  //   9: Lab color
  if (stbi__get16be(s) != 3)
    return stbi__errpuc("wrong color format", "PSD is not in RGB color format");

  // Skip the Mode Data.  (It's the palette for indexed color; other info for
  // other modes.)
  stbi__skip(s, stbi__get32be(s));

  // Skip the image resources.  (resolution, pen tool paths, etc)
  stbi__skip(s, stbi__get32be(s));

  // Skip the reserved data.
  stbi__skip(s, stbi__get32be(s));

  // Find out if the data is compressed.
  // Known values:
  //   0: no compression
  //   1: RLE compressed
  compression = stbi__get16be(s);
  if (compression > 1)
    return stbi__errpuc("bad compression",
                        "PSD has an unknown compression format");

  // Check size
  if (!stbi__mad3sizes_valid(4, w, h, 0))
    return stbi__errpuc("too large", "Corrupt PSD");

  // Create the destination image.

  if (!compression && bitdepth == 16 && bpc == 16) {
    out = (stbi_uc *)stbi__malloc_mad3(8, w, h, 0);
    ri->bits_per_channel = 16;
  } else
    out = (stbi_uc *)stbi__malloc(4 * w * h);

  if (!out)
    return stbi__errpuc("outofmem", "Out of memory");
  pixelCount = w * h;

  // Initialize the data to zero.
  // memset( out, 0, pixelCount * 4 );

  // Finally, the image data.
  if (compression) {
    // RLE as used by .PSD and .TIFF
    // Loop until you get the number of unpacked bytes you are expecting:
    //     Read the next source byte into n.
    //     If n is between 0 and 127 inclusive, copy the next n+1 bytes
    //     literally. Else if n is between -127 and -1 inclusive, copy the next
    //     byte -n+1 times. Else if n is 128, noop.
    // Endloop

    // The RLE-compressed data is preceeded by a 2-byte data count for each row
    // in the data, which we're going to just skip.
    stbi__skip(s, h * channelCount * 2);

    // Read the RLE data by channel.
    for (channel = 0; channel < 4; channel++) {
      stbi_uc *p;

      p = out + channel;
      if (channel >= channelCount) {
        // Fill this channel with default data.
        for (i = 0; i < pixelCount; i++, p += 4)
          *p = (channel == 3 ? 255 : 0);
      } else {
        // Read the RLE data.
        if (!stbi__psd_decode_rle(s, p, pixelCount)) {
          STBI_FREE(out);
          return stbi__errpuc("corrupt", "bad RLE data");
        }
      }
    }

  } else {
    // We're at the raw image data.  It's each channel in order (Red, Green,
    // Blue, Alpha, ...) where each channel consists of an 8-bit (or 16-bit)
    // value for each pixel in the image.

    // Read the data by channel.
    for (channel = 0; channel < 4; channel++) {
      if (channel >= channelCount) {
        // Fill this channel with default data.
        if (bitdepth == 16 && bpc == 16) {
          stbi__uint16 *q = ((stbi__uint16 *)out) + channel;
          stbi__uint16 val = channel == 3 ? 65535 : 0;
          for (i = 0; i < pixelCount; i++, q += 4)
            *q = val;
        } else {
          stbi_uc *p = out + channel;
          stbi_uc val = channel == 3 ? 255 : 0;
          for (i = 0; i < pixelCount; i++, p += 4)
            *p = val;
        }
      } else {
        if (ri->bits_per_channel == 16) { // output bpc
          stbi__uint16 *q = ((stbi__uint16 *)out) + channel;
          for (i = 0; i < pixelCount; i++, q += 4)
            *q = (stbi__uint16)stbi__get16be(s);
        } else {
          stbi_uc *p = out + channel;
          if (bitdepth == 16) { // input bpc
            for (i = 0; i < pixelCount; i++, p += 4)
              *p = (stbi_uc)(stbi__get16be(s) >> 8);
          } else {
            for (i = 0; i < pixelCount; i++, p += 4)
              *p = stbi__get8(s);
          }
        }
      }
    }
  }

  // remove weird white matte from PSD
  if (channelCount >= 4) {
    if (ri->bits_per_channel == 16) {
      for (i = 0; i < w * h; ++i) {
        stbi__uint16 *pixel = (stbi__uint16 *)out + 4 * i;
        if (pixel[3] != 0 && pixel[3] != 65535) {
          float a = pixel[3] / 65535.0f;
          float ra = 1.0f / a;
          float inv_a = 65535.0f * (1 - ra);
          pixel[0] = (stbi__uint16)(pixel[0] * ra + inv_a);
          pixel[1] = (stbi__uint16)(pixel[1] * ra + inv_a);
          pixel[2] = (stbi__uint16)(pixel[2] * ra + inv_a);
        }
      }
    } else {
      for (i = 0; i < w * h; ++i) {
        unsigned char *pixel = out + 4 * i;
        if (pixel[3] != 0 && pixel[3] != 255) {
          float a = pixel[3] / 255.0f;
          float ra = 1.0f / a;
          float inv_a = 255.0f * (1 - ra);
          pixel[0] = (unsigned char)(pixel[0] * ra + inv_a);
          pixel[1] = (unsigned char)(pixel[1] * ra + inv_a);
          pixel[2] = (unsigned char)(pixel[2] * ra + inv_a);
        }
      }
    }
  }

  // convert to desired output format
  if (req_comp && req_comp != 4) {
    if (ri->bits_per_channel == 16)
      out = (stbi_uc *)stbi__convert_format16((stbi__uint16 *)out, 4, req_comp,
                                              w, h);
    else
      out = stbi__convert_format(out, 4, req_comp, w, h);
    if (out == NULL)
      return out; // stbi__convert_format frees input on failure
  }

  if (comp)
    *comp = 4;
  *y = h;
  *x = w;

  return out;
}
#endif

  // *************************************************************************************************
  // Softimage PIC loader
  // by Tom Seddon
  //
  // See http://softimage.wiki.softimage.com/index.php/INFO:_PIC_file_format
  // See http://ozviz.wasp.uwa.edu.au/~pbourke/dataformats/softimagepic/

#ifndef STBI_NO_PIC
static int stbi__pic_is4(stbi__context *s, const char *str) {
  int i;
  for (i = 0; i < 4; ++i)
    if (stbi__get8(s) != (stbi_uc)str[i])
      return 0;

  return 1;
}

static int stbi__pic_test_core(stbi__context *s) {
  int i;

  if (!stbi__pic_is4(s, "\x53\x80\xF6\x34"))
    return 0;

  for (i = 0; i < 84; ++i)
    stbi__get8(s);

  if (!stbi__pic_is4(s, "PICT"))
    return 0;

  return 1;
}

typedef struct {
  stbi_uc size, type, channel;
} stbi__pic_packet;

static stbi_uc *stbi__readval(stbi__context *s, int channel, stbi_uc *dest) {
  int mask = 0x80, i;

  for (i = 0; i < 4; ++i, mask >>= 1) {
    if (channel & mask) {
      if (stbi__at_eof(s))
        return stbi__errpuc("bad file", "PIC file too short");
      dest[i] = stbi__get8(s);
    }
  }

  return dest;
}

static void stbi__copyval(int channel, stbi_uc *dest, const stbi_uc *src) {
  int mask = 0x80, i;

  for (i = 0; i < 4; ++i, mask >>= 1)
    if (channel & mask)
      dest[i] = src[i];
}

static stbi_uc *stbi__pic_load_core(stbi__context *s, int width, int height,
                                    int *comp, stbi_uc *result) {
  int act_comp = 0, num_packets = 0, y, chained;
  stbi__pic_packet packets[10];

  // this will (should...) cater for even some bizarre stuff like having data
  // for the same channel in multiple packets.
  do {
    stbi__pic_packet *packet;

    if (num_packets == sizeof(packets) / sizeof(packets[0]))
      return stbi__errpuc("bad format", "too many packets");

    packet = &packets[num_packets++];

    chained = stbi__get8(s);
    packet->size = stbi__get8(s);
    packet->type = stbi__get8(s);
    packet->channel = stbi__get8(s);

    act_comp |= packet->channel;

    if (stbi__at_eof(s))
      return stbi__errpuc("bad file", "file too short (reading packets)");
    if (packet->size != 8)
      return stbi__errpuc("bad format", "packet isn't 8bpp");
  } while (chained);

  *comp = (act_comp & 0x10 ? 4 : 3); // has alpha channel?

  for (y = 0; y < height; ++y) {
    int packet_idx;

    for (packet_idx = 0; packet_idx < num_packets; ++packet_idx) {
      stbi__pic_packet *packet = &packets[packet_idx];
      stbi_uc *dest = result + y * width * 4;

      switch (packet->type) {
      default:
        return stbi__errpuc("bad format", "packet has bad compression type");

      case 0: { // uncompressed
        int x;

        for (x = 0; x < width; ++x, dest += 4)
          if (!stbi__readval(s, packet->channel, dest))
            return 0;
        break;
      }

      case 1: // Pure RLE
      {
        int left = width, i;

        while (left > 0) {
          stbi_uc count, value[4];

          count = stbi__get8(s);
          if (stbi__at_eof(s))
            return stbi__errpuc("bad file", "file too short (pure read count)");

          if (count > left)
            count = (stbi_uc)left;

          if (!stbi__readval(s, packet->channel, value))
            return 0;

          for (i = 0; i < count; ++i, dest += 4)
            stbi__copyval(packet->channel, dest, value);
          left -= count;
        }
      } break;

      case 2: { // Mixed RLE
        int left = width;
        while (left > 0) {
          int count = stbi__get8(s), i;
          if (stbi__at_eof(s))
            return stbi__errpuc("bad file",
                                "file too short (mixed read count)");

          if (count >= 128) { // Repeated
            stbi_uc value[4];

            if (count == 128)
              count = stbi__get16be(s);
            else
              count -= 127;
            if (count > left)
              return stbi__errpuc("bad file", "scanline overrun");

            if (!stbi__readval(s, packet->channel, value))
              return 0;

            for (i = 0; i < count; ++i, dest += 4)
              stbi__copyval(packet->channel, dest, value);
          } else { // Raw
            ++count;
            if (count > left)
              return stbi__errpuc("bad file", "scanline overrun");

            for (i = 0; i < count; ++i, dest += 4)
              if (!stbi__readval(s, packet->channel, dest))
                return 0;
          }
          left -= count;
        }
        break;
      }
      }
    }
  }

  return result;
}

static void *stbi__pic_load(stbi__context *s, int *px, int *py, int *comp,
                            int req_comp, stbi__result_info *ri) {
  stbi_uc *result;
  int i, x, y, internal_comp;
  STBI_NOTUSED(ri);

  if (!comp)
    comp = &internal_comp;

  for (i = 0; i < 92; ++i)
    stbi__get8(s);

  x = stbi__get16be(s);
  y = stbi__get16be(s);
  if (stbi__at_eof(s))
    return stbi__errpuc("bad file", "file too short (pic header)");
  if (!stbi__mad3sizes_valid(x, y, 4, 0))
    return stbi__errpuc("too large", "PIC image too large to decode");

  stbi__get32be(s); // skip `ratio'
  stbi__get16be(s); // skip `fields'
  stbi__get16be(s); // skip `pad'

  // intermediate buffer is RGBA
  result = (stbi_uc *)stbi__malloc_mad3(x, y, 4, 0);
  memset(result, 0xff, x * y * 4);

  if (!stbi__pic_load_core(s, x, y, comp, result)) {
    STBI_FREE(result);
    result = 0;
  }
  *px = x;
  *py = y;
  if (req_comp == 0)
    req_comp = *comp;
  result = stbi__convert_format(result, 4, req_comp, x, y);

  return result;
}

static int stbi__pic_test(stbi__context *s) {
  int r = stbi__pic_test_core(s);
  stbi__rewind(s);
  return r;
}
#endif

  // *************************************************************************************************
  // GIF loader -- public domain by Jean-Marc Lienher -- simplified/shrunk by
  // stb

#ifndef STBI_NO_GIF
typedef struct {
  stbi__int16 prefix;
  stbi_uc first;
  stbi_uc suffix;
} stbi__gif_lzw;

typedef struct {
  int w, h;
  stbi_uc *out, *old_out; // output buffer (always 4 components)
  int flags, bgindex, ratio, transparent, eflags, delay;
  stbi_uc pal[256][4];
  stbi_uc lpal[256][4];
  stbi__gif_lzw codes[4096];
  stbi_uc *color_table;
  int parse, step;
  int lflags;
  int start_x, start_y;
  int max_x, max_y;
  int cur_x, cur_y;
  int line_size;
} stbi__gif;

static int stbi__gif_test_raw(stbi__context *s) {
  int sz;
  if (stbi__get8(s) != 'G' || stbi__get8(s) != 'I' || stbi__get8(s) != 'F' ||
      stbi__get8(s) != '8')
    return 0;
  sz = stbi__get8(s);
  if (sz != '9' && sz != '7')
    return 0;
  if (stbi__get8(s) != 'a')
    return 0;
  return 1;
}

static int stbi__gif_test(stbi__context *s) {
  int r = stbi__gif_test_raw(s);
  stbi__rewind(s);
  return r;
}

static void stbi__gif_parse_colortable(stbi__context *s, stbi_uc pal[256][4],
                                       int num_entries, int transp) {
  int i;
  for (i = 0; i < num_entries; ++i) {
    pal[i][2] = stbi__get8(s);
    pal[i][1] = stbi__get8(s);
    pal[i][0] = stbi__get8(s);
    pal[i][3] = transp == i ? 0 : 255;
  }
}

static int stbi__gif_header(stbi__context *s, stbi__gif *g, int *comp,
                            int is_info) {
  stbi_uc version;
  if (stbi__get8(s) != 'G' || stbi__get8(s) != 'I' || stbi__get8(s) != 'F' ||
      stbi__get8(s) != '8')
    return stbi__err("not GIF", "Corrupt GIF");

  version = stbi__get8(s);
  if (version != '7' && version != '9')
    return stbi__err("not GIF", "Corrupt GIF");
  if (stbi__get8(s) != 'a')
    return stbi__err("not GIF", "Corrupt GIF");

  stbi__g_failure_reason = "";
  g->w = stbi__get16le(s);
  g->h = stbi__get16le(s);
  g->flags = stbi__get8(s);
  g->bgindex = stbi__get8(s);
  g->ratio = stbi__get8(s);
  g->transparent = -1;

  if (comp != 0)
    *comp = 4; // can't actually tell whether it's 3 or 4 until we parse the
               // comments

  if (is_info)
    return 1;

  if (g->flags & 0x80)
    stbi__gif_parse_colortable(s, g->pal, 2 << (g->flags & 7), -1);

  return 1;
}

static int stbi__gif_info_raw(stbi__context *s, int *x, int *y, int *comp) {
  stbi__gif *g = (stbi__gif *)stbi__malloc(sizeof(stbi__gif));
  if (!stbi__gif_header(s, g, comp, 1)) {
    STBI_FREE(g);
    stbi__rewind(s);
    return 0;
  }
  if (x)
    *x = g->w;
  if (y)
    *y = g->h;
  STBI_FREE(g);
  return 1;
}

static void stbi__out_gif_code(stbi__gif *g, stbi__uint16 code) {
  stbi_uc *p, *c;

  // recurse to decode the prefixes, since the linked-list is backwards,
  // and working backwards through an interleaved image would be nasty
  if (g->codes[code].prefix >= 0)
    stbi__out_gif_code(g, g->codes[code].prefix);

  if (g->cur_y >= g->max_y)
    return;

  p = &g->out[g->cur_x + g->cur_y];
  c = &g->color_table[g->codes[code].suffix * 4];

  if (c[3] >= 128) {
    p[0] = c[2];
    p[1] = c[1];
    p[2] = c[0];
    p[3] = c[3];
  }
  g->cur_x += 4;

  if (g->cur_x >= g->max_x) {
    g->cur_x = g->start_x;
    g->cur_y += g->step;

    while (g->cur_y >= g->max_y && g->parse > 0) {
      g->step = (1 << g->parse) * g->line_size;
      g->cur_y = g->start_y + (g->step >> 1);
      --g->parse;
    }
  }
}

static stbi_uc *stbi__process_gif_raster(stbi__context *s, stbi__gif *g) {
  stbi_uc lzw_cs;
  stbi__int32 len, init_code;
  stbi__uint32 first;
  stbi__int32 codesize, codemask, avail, oldcode, bits, valid_bits, clear;
  stbi__gif_lzw *p;

  lzw_cs = stbi__get8(s);
  if (lzw_cs > 12)
    return NULL;
  clear = 1 << lzw_cs;
  first = 1;
  codesize = lzw_cs + 1;
  codemask = (1 << codesize) - 1;
  bits = 0;
  valid_bits = 0;
  for (init_code = 0; init_code < clear; init_code++) {
    g->codes[init_code].prefix = -1;
    g->codes[init_code].first = (stbi_uc)init_code;
    g->codes[init_code].suffix = (stbi_uc)init_code;
  }

  // support no starting clear code
  avail = clear + 2;
  oldcode = -1;

  len = 0;
  for (;;) {
    if (valid_bits < codesize) {
      if (len == 0) {
        len = stbi__get8(s); // start new block
        if (len == 0)
          return g->out;
      }
      --len;
      bits |= (stbi__int32)stbi__get8(s) << valid_bits;
      valid_bits += 8;
    } else {
      stbi__int32 code = bits & codemask;
      bits >>= codesize;
      valid_bits -= codesize;
      // @OPTIMIZE: is there some way we can accelerate the non-clear path?
      if (code == clear) { // clear code
        codesize = lzw_cs + 1;
        codemask = (1 << codesize) - 1;
        avail = clear + 2;
        oldcode = -1;
        first = 0;
      } else if (code == clear + 1) { // end of stream code
        stbi__skip(s, len);
        while ((len = stbi__get8(s)) > 0)
          stbi__skip(s, len);
        return g->out;
      } else if (code <= avail) {
        if (first)
          return stbi__errpuc("no clear code", "Corrupt GIF");

        if (oldcode >= 0) {
          p = &g->codes[avail++];
          if (avail > 4096)
            return stbi__errpuc("too many codes", "Corrupt GIF");
          p->prefix = (stbi__int16)oldcode;
          p->first = g->codes[oldcode].first;
          p->suffix = (code == avail) ? p->first : g->codes[code].first;
        } else if (code == avail)
          return stbi__errpuc("illegal code in raster", "Corrupt GIF");

        stbi__out_gif_code(g, (stbi__uint16)code);

        if ((avail & codemask) == 0 && avail <= 0x0FFF) {
          codesize++;
          codemask = (1 << codesize) - 1;
        }

        oldcode = code;
      } else {
        return stbi__errpuc("illegal code in raster", "Corrupt GIF");
      }
    }
  }
}

static void stbi__fill_gif_background(stbi__gif *g, int x0, int y0, int x1,
                                      int y1) {
  int x, y;
  stbi_uc *c = g->pal[g->bgindex];
  for (y = y0; y < y1; y += 4 * g->w) {
    for (x = x0; x < x1; x += 4) {
      stbi_uc *p = &g->out[y + x];
      p[0] = c[2];
      p[1] = c[1];
      p[2] = c[0];
      p[3] = 0;
    }
  }
}

// this function is designed to support animated gifs, although stb_image
// doesn't support it
static stbi_uc *stbi__gif_load_next(stbi__context *s, stbi__gif *g, int *comp,
                                    int req_comp) {
  int i;
  stbi_uc *prev_out = 0;

  if (g->out == 0 && !stbi__gif_header(s, g, comp, 0))
    return 0; // stbi__g_failure_reason set by stbi__gif_header

  if (!stbi__mad3sizes_valid(g->w, g->h, 4, 0))
    return stbi__errpuc("too large", "GIF too large");

  prev_out = g->out;
  g->out = (stbi_uc *)stbi__malloc_mad3(4, g->w, g->h, 0);
  if (g->out == 0)
    return stbi__errpuc("outofmem", "Out of memory");

  switch ((g->eflags & 0x1C) >> 2) {
  case 0: // unspecified (also always used on 1st frame)
    stbi__fill_gif_background(g, 0, 0, 4 * g->w, 4 * g->w * g->h);
    break;
  case 1: // do not dispose
    if (prev_out)
      memcpy(g->out, prev_out, 4 * g->w * g->h);
    g->old_out = prev_out;
    break;
  case 2: // dispose to background
    if (prev_out)
      memcpy(g->out, prev_out, 4 * g->w * g->h);
    stbi__fill_gif_background(g, g->start_x, g->start_y, g->max_x, g->max_y);
    break;
  case 3: // dispose to previous
    if (g->old_out) {
      for (i = g->start_y; i < g->max_y; i += 4 * g->w)
        memcpy(&g->out[i + g->start_x], &g->old_out[i + g->start_x],
               g->max_x - g->start_x);
    }
    break;
  }

  for (;;) {
    switch (stbi__get8(s)) {
    case 0x2C: /* Image Descriptor */
    {
      int prev_trans = -1;
      stbi__int32 x, y, w, h;
      stbi_uc *o;

      x = stbi__get16le(s);
      y = stbi__get16le(s);
      w = stbi__get16le(s);
      h = stbi__get16le(s);
      if (((x + w) > (g->w)) || ((y + h) > (g->h)))
        return stbi__errpuc("bad Image Descriptor", "Corrupt GIF");

      g->line_size = g->w * 4;
      g->start_x = x * 4;
      g->start_y = y * g->line_size;
      g->max_x = g->start_x + w * 4;
      g->max_y = g->start_y + h * g->line_size;
      g->cur_x = g->start_x;
      g->cur_y = g->start_y;

      g->lflags = stbi__get8(s);

      if (g->lflags & 0x40) {
        g->step = 8 * g->line_size; // first interlaced spacing
        g->parse = 3;
      } else {
        g->step = g->line_size;
        g->parse = 0;
      }

      if (g->lflags & 0x80) {
        stbi__gif_parse_colortable(s, g->lpal, 2 << (g->lflags & 7),
                                   g->eflags & 0x01 ? g->transparent : -1);
        g->color_table = (stbi_uc *)g->lpal;
      } else if (g->flags & 0x80) {
        if (g->transparent >= 0 && (g->eflags & 0x01)) {
          prev_trans = g->pal[g->transparent][3];
          g->pal[g->transparent][3] = 0;
        }
        g->color_table = (stbi_uc *)g->pal;
      } else
        return stbi__errpuc("missing color table", "Corrupt GIF");

      o = stbi__process_gif_raster(s, g);
      if (o == NULL)
        return NULL;

      if (prev_trans != -1)
        g->pal[g->transparent][3] = (stbi_uc)prev_trans;

      return o;
    }

    case 0x21: // Comment Extension.
    {
      int len;
      if (stbi__get8(s) == 0xF9) { // Graphic Control Extension.
        len = stbi__get8(s);
        if (len == 4) {
          g->eflags = stbi__get8(s);
          g->delay = stbi__get16le(s);
          g->transparent = stbi__get8(s);
        } else {
          stbi__skip(s, len);
          break;
        }
      }
      while ((len = stbi__get8(s)) != 0)
        stbi__skip(s, len);
      break;
    }

    case 0x3B:             // gif stream termination code
      return (stbi_uc *)s; // using '1' causes warning on some compilers

    default:
      return stbi__errpuc("unknown code", "Corrupt GIF");
    }
  }

  STBI_NOTUSED(req_comp);
}

static void *stbi__gif_load(stbi__context *s, int *x, int *y, int *comp,
                            int req_comp, stbi__result_info *ri) {
  stbi_uc *u = 0;
  stbi__gif *g = (stbi__gif *)stbi__malloc(sizeof(stbi__gif));
  memset(g, 0, sizeof(*g));
  STBI_NOTUSED(ri);

  u = stbi__gif_load_next(s, g, comp, req_comp);
  if (u == (stbi_uc *)s)
    u = 0; // end of animated gif marker
  if (u) {
    *x = g->w;
    *y = g->h;
    if (req_comp && req_comp != 4)
      u = stbi__convert_format(u, 4, req_comp, g->w, g->h);
  } else if (g->out)
    STBI_FREE(g->out);
  STBI_FREE(g);
  return u;
}

static int stbi__gif_info(stbi__context *s, int *x, int *y, int *comp) {
  return stbi__gif_info_raw(s, x, y, comp);
}
#endif

// *************************************************************************************************
// Radiance RGBE HDR loader
// originally by Nicolas Schulz
#ifndef STBI_NO_HDR
static int stbi__hdr_test_core(stbi__context *s, const char *signature) {
  int i;
  for (i = 0; signature[i]; ++i)
    if (stbi__get8(s) != signature[i])
      return 0;
  stbi__rewind(s);
  return 1;
}

static int stbi__hdr_test(stbi__context *s) {
  int r = stbi__hdr_test_core(s, "#?RADIANCE\n");
  stbi__rewind(s);
  if (!r) {
    r = stbi__hdr_test_core(s, "#?RGBE\n");
    stbi__rewind(s);
  }
  return r;
}

#define STBI__HDR_BUFLEN 1024
static char *stbi__hdr_gettoken(stbi__context *z, char *buffer) {
  int len = 0;
  char c = '\0';

  c = (char)stbi__get8(z);

  while (!stbi__at_eof(z) && c != '\n') {
    buffer[len++] = c;
    if (len == STBI__HDR_BUFLEN - 1) {
      // flush to end of line
      while (!stbi__at_eof(z) && stbi__get8(z) != '\n')
        ;
      break;
    }
    c = (char)stbi__get8(z);
  }

  buffer[len] = 0;
  return buffer;
}

static void stbi__hdr_convert(float *output, stbi_uc *input, int req_comp) {
  if (input[3] != 0) {
    float f1;
    // Exponent
    f1 = (float)ldexp(1.0f, input[3] - (int)(128 + 8));
    if (req_comp <= 2)
      output[0] = (input[0] + input[1] + input[2]) * f1 / 3;
    else {
      output[0] = input[0] * f1;
      output[1] = input[1] * f1;
      output[2] = input[2] * f1;
    }
    if (req_comp == 2)
      output[1] = 1;
    if (req_comp == 4)
      output[3] = 1;
  } else {
    switch (req_comp) {
    case 4:
      output[3] = 1;
      output[0] = output[1] = output[2] = 0;
      break; // GIVR: Line above and this one are new. silence a warning.
    case 3:
      output[0] = output[1] = output[2] = 0;
      break;
    case 2:
      output[1] = 1;
      output[0] = 0;
      break; // GIVR: Line above and this one are new. silence a warning.
    case 1:
      output[0] = 0;
      break;
    }
  }
}

static float *stbi__hdr_load(stbi__context *s, int *x, int *y, int *comp,
                             int req_comp, stbi__result_info *ri) {
  char buffer[STBI__HDR_BUFLEN];
  char *token;
  int valid = 0;
  int width, height;
  stbi_uc *scanline;
  float *hdr_data;
  int len;
  unsigned char count, value;
  int i, j, k, c1, c2, z;
  const char *headerToken;
  STBI_NOTUSED(ri);

  // Check identifier
  headerToken = stbi__hdr_gettoken(s, buffer);
  if (strcmp(headerToken, "#?RADIANCE") != 0 &&
      strcmp(headerToken, "#?RGBE") != 0)
    return stbi__errpf("not HDR", "Corrupt HDR image");

  // Parse header
  for (;;) {
    token = stbi__hdr_gettoken(s, buffer);
    if (token[0] == 0)
      break;
    if (strcmp(token, "FORMAT=32-bit_rle_rgbe") == 0)
      valid = 1;
  }

  if (!valid)
    return stbi__errpf("unsupported format", "Unsupported HDR format");

  // Parse width and height
  // can't use sscanf() if we're not using stdio!
  token = stbi__hdr_gettoken(s, buffer);
  if (strncmp(token, "-Y ", 3))
    return stbi__errpf("unsupported data layout", "Unsupported HDR format");
  token += 3;
  height = (int)strtol(token, &token, 10);
  while (*token == ' ')
    ++token;
  if (strncmp(token, "+X ", 3))
    return stbi__errpf("unsupported data layout", "Unsupported HDR format");
  token += 3;
  width = (int)strtol(token, NULL, 10);

  *x = width;
  *y = height;

  if (comp)
    *comp = 3;
  if (req_comp == 0)
    req_comp = 3;

  if (!stbi__mad4sizes_valid(width, height, req_comp, sizeof(float), 0))
    return stbi__errpf("too large", "HDR image is too large");

  // Read data
  hdr_data =
      (float *)stbi__malloc_mad4(width, height, req_comp, sizeof(float), 0);
  if (!hdr_data)
    return stbi__errpf("outofmem", "Out of memory");

  // Load image data
  // image data is stored as some number of sca
  if (width < 8 || width >= 32768) {
    // Read flat data
    for (j = 0; j < height; ++j) {
      for (i = 0; i < width; ++i) {
        stbi_uc rgbe[4];
      main_decode_loop:
        stbi__getn(s, rgbe, 4);
        stbi__hdr_convert(hdr_data + j * width * req_comp + i * req_comp, rgbe,
                          req_comp);
      }
    }
  } else {
    // Read RLE-encoded data
    scanline = NULL;

    for (j = 0; j < height; ++j) {
      c1 = stbi__get8(s);
      c2 = stbi__get8(s);
      len = stbi__get8(s);
      if (c1 != 2 || c2 != 2 || (len & 0x80)) {
        // not run-length encoded, so we have to actually use THIS data as a
        // decoded pixel (note this can't be a valid pixel--one of RGB must be
        // >= 128)
        stbi_uc rgbe[4];
        rgbe[0] = (stbi_uc)c1;
        rgbe[1] = (stbi_uc)c2;
        rgbe[2] = (stbi_uc)len;
        rgbe[3] = (stbi_uc)stbi__get8(s);
        stbi__hdr_convert(hdr_data, rgbe, req_comp);
        i = 1;
        j = 0;
        STBI_FREE(scanline);
        goto main_decode_loop; // yes, this makes no sense
      }
      len <<= 8;
      len |= stbi__get8(s);
      if (len != width) {
        STBI_FREE(hdr_data);
        STBI_FREE(scanline);
        return stbi__errpf("invalid decoded scanline length", "corrupt HDR");
      }
      if (scanline == NULL) {
        scanline = (stbi_uc *)stbi__malloc_mad2(width, 4, 0);
        if (!scanline) {
          STBI_FREE(hdr_data);
          return stbi__errpf("outofmem", "Out of memory");
        }
      }

      for (k = 0; k < 4; ++k) {
        int nleft;
        i = 0;
        while ((nleft = width - i) > 0) {
          count = stbi__get8(s);
          if (count > 128) {
            // Run
            value = stbi__get8(s);
            count -= 128;
            if (count > nleft) {
              STBI_FREE(hdr_data);
              STBI_FREE(scanline);
              return stbi__errpf("corrupt", "bad RLE data in HDR");
            }
            for (z = 0; z < count; ++z)
              scanline[i++ * 4 + k] = value;
          } else {
            // Dump
            if (count > nleft) {
              STBI_FREE(hdr_data);
              STBI_FREE(scanline);
              return stbi__errpf("corrupt", "bad RLE data in HDR");
            }
            for (z = 0; z < count; ++z)
              scanline[i++ * 4 + k] = stbi__get8(s);
          }
        }
      }
      for (i = 0; i < width; ++i)
        stbi__hdr_convert(hdr_data + (j * width + i) * req_comp,
                          scanline + i * 4, req_comp);
    }
    if (scanline)
      STBI_FREE(scanline);
  }

  return hdr_data;
}

static int stbi__hdr_info(stbi__context *s, int *x, int *y, int *comp) {
  char buffer[STBI__HDR_BUFLEN];
  char *token;
  int valid = 0;
  int dummy;

  if (!x)
    x = &dummy;
  if (!y)
    y = &dummy;
  if (!comp)
    comp = &dummy;

  if (stbi__hdr_test(s) == 0) {
    stbi__rewind(s);
    return 0;
  }

  for (;;) {
    token = stbi__hdr_gettoken(s, buffer);
    if (token[0] == 0)
      break;
    if (strcmp(token, "FORMAT=32-bit_rle_rgbe") == 0)
      valid = 1;
  }

  if (!valid) {
    stbi__rewind(s);
    return 0;
  }
  token = stbi__hdr_gettoken(s, buffer);
  if (strncmp(token, "-Y ", 3)) {
    stbi__rewind(s);
    return 0;
  }
  token += 3;
  *y = (int)strtol(token, &token, 10);
  while (*token == ' ')
    ++token;
  if (strncmp(token, "+X ", 3)) {
    stbi__rewind(s);
    return 0;
  }
  token += 3;
  *x = (int)strtol(token, NULL, 10);
  *comp = 3;
  return 1;
}
#endif // STBI_NO_HDR

#ifndef STBI_NO_BMP
static int stbi__bmp_info(stbi__context *s, int *x, int *y, int *comp) {
  void *p;
  stbi__bmp_data info;

  info.all_a = 255;
  p = stbi__bmp_parse_header(s, &info);
  stbi__rewind(s);
  if (p == NULL)
    return 0;
  if (x)
    *x = s->img_x;
  if (y)
    *y = s->img_y;
  if (comp)
    *comp = info.ma ? 4 : 3;
  return 1;
}
#endif

#ifndef STBI_NO_PSD
static int stbi__psd_info(stbi__context *s, int *x, int *y, int *comp) {
  int channelCount, dummy;
  if (!x)
    x = &dummy;
  if (!y)
    y = &dummy;
  if (!comp)
    comp = &dummy;
  if (stbi__get32be(s) != 0x38425053) {
    stbi__rewind(s);
    return 0;
  }
  if (stbi__get16be(s) != 1) {
    stbi__rewind(s);
    return 0;
  }
  stbi__skip(s, 6);
  channelCount = stbi__get16be(s);
  if (channelCount < 0 || channelCount > 16) {
    stbi__rewind(s);
    return 0;
  }
  *y = stbi__get32be(s);
  *x = stbi__get32be(s);
  if (stbi__get16be(s) != 8) {
    stbi__rewind(s);
    return 0;
  }
  if (stbi__get16be(s) != 3) {
    stbi__rewind(s);
    return 0;
  }
  *comp = 4;
  return 1;
}
#endif

#ifndef STBI_NO_PIC
static int stbi__pic_info(stbi__context *s, int *x, int *y, int *comp) {
  int act_comp = 0, num_packets = 0, chained, dummy;
  stbi__pic_packet packets[10];

  if (!x)
    x = &dummy;
  if (!y)
    y = &dummy;
  if (!comp)
    comp = &dummy;

  if (!stbi__pic_is4(s, "\x53\x80\xF6\x34")) {
    stbi__rewind(s);
    return 0;
  }

  stbi__skip(s, 88);

  *x = stbi__get16be(s);
  *y = stbi__get16be(s);
  if (stbi__at_eof(s)) {
    stbi__rewind(s);
    return 0;
  }
  if ((*x) != 0 && (1 << 28) / (*x) < (*y)) {
    stbi__rewind(s);
    return 0;
  }

  stbi__skip(s, 8);

  do {
    stbi__pic_packet *packet;

    if (num_packets == sizeof(packets) / sizeof(packets[0]))
      return 0;

    packet = &packets[num_packets++];
    chained = stbi__get8(s);
    packet->size = stbi__get8(s);
    packet->type = stbi__get8(s);
    packet->channel = stbi__get8(s);
    act_comp |= packet->channel;

    if (stbi__at_eof(s)) {
      stbi__rewind(s);
      return 0;
    }
    if (packet->size != 8) {
      stbi__rewind(s);
      return 0;
    }
  } while (chained);

  *comp = (act_comp & 0x10 ? 4 : 3);

  return 1;
}
#endif

  // *************************************************************************************************
  // Portable Gray Map and Portable Pixel Map loader
  // by Ken Miller
  //
  // PGM: http://netpbm.sourceforge.net/doc/pgm.html
  // PPM: http://netpbm.sourceforge.net/doc/ppm.html
  //
  // Known limitations:
  //    Does not support comments in the header section
  //    Does not support ASCII image data (formats P2 and P3)
  //    Does not support 16-bit-per-channel

#ifndef STBI_NO_PNM

static int stbi__pnm_test(stbi__context *s) {
  char p, t;
  p = (char)stbi__get8(s);
  t = (char)stbi__get8(s);
  if (p != 'P' || (t != '5' && t != '6')) {
    stbi__rewind(s);
    return 0;
  }
  return 1;
}

static void *stbi__pnm_load(stbi__context *s, int *x, int *y, int *comp,
                            int req_comp, stbi__result_info *ri) {
  stbi_uc *out;
  STBI_NOTUSED(ri);

  if (!stbi__pnm_info(s, (int *)&s->img_x, (int *)&s->img_y, (int *)&s->img_n))
    return 0;

  *x = s->img_x;
  *y = s->img_y;
  if (comp)
    *comp = s->img_n;

  if (!stbi__mad3sizes_valid(s->img_n, s->img_x, s->img_y, 0))
    return stbi__errpuc("too large", "PNM too large");

  out = (stbi_uc *)stbi__malloc_mad3(s->img_n, s->img_x, s->img_y, 0);
  if (!out)
    return stbi__errpuc("outofmem", "Out of memory");
  stbi__getn(s, out, s->img_n * s->img_x * s->img_y);

  if (req_comp && req_comp != s->img_n) {
    out = stbi__convert_format(out, s->img_n, req_comp, s->img_x, s->img_y);
    if (out == NULL)
      return out; // stbi__convert_format frees input on failure
  }
  return out;
}

static int stbi__pnm_isspace(char c) {
  return c == ' ' || c == '\t' || c == '\n' || c == '\v' || c == '\f' ||
         c == '\r';
}

static void stbi__pnm_skip_whitespace(stbi__context *s, char *c) {
  for (;;) {
    while (!stbi__at_eof(s) && stbi__pnm_isspace(*c))
      *c = (char)stbi__get8(s);

    if (stbi__at_eof(s) || *c != '#')
      break;

    while (!stbi__at_eof(s) && *c != '\n' && *c != '\r')
      *c = (char)stbi__get8(s);
  }
}

static int stbi__pnm_isdigit(char c) { return c >= '0' && c <= '9'; }

static int stbi__pnm_getinteger(stbi__context *s, char *c) {
  int value = 0;

  while (!stbi__at_eof(s) && stbi__pnm_isdigit(*c)) {
    value = value * 10 + (*c - '0');
    *c = (char)stbi__get8(s);
  }

  return value;
}

static int stbi__pnm_info(stbi__context *s, int *x, int *y, int *comp) {
  int maxv, dummy;
  char c, p, t;

  if (!x)
    x = &dummy;
  if (!y)
    y = &dummy;
  if (!comp)
    comp = &dummy;

  stbi__rewind(s);

  // Get identifier
  p = (char)stbi__get8(s);
  t = (char)stbi__get8(s);
  if (p != 'P' || (t != '5' && t != '6')) {
    stbi__rewind(s);
    return 0;
  }

  *comp =
      (t == '6') ? 3 : 1; // '5' is 1-component .pgm; '6' is 3-component .ppm

  c = (char)stbi__get8(s);
  stbi__pnm_skip_whitespace(s, &c);

  *x = stbi__pnm_getinteger(s, &c); // read width
  stbi__pnm_skip_whitespace(s, &c);

  *y = stbi__pnm_getinteger(s, &c); // read height
  stbi__pnm_skip_whitespace(s, &c);

  maxv = stbi__pnm_getinteger(s, &c); // read max value

  if (maxv > 255)
    return stbi__err("max value > 255", "PPM image not 8-bit");
  else
    return 1;
}
#endif

static int stbi__info_main(stbi__context *s, int *x, int *y, int *comp) {
#ifndef STBI_NO_JPEG
  if (stbi__jpeg_info(s, x, y, comp))
    return 1;
#endif

#ifndef STBI_NO_PNG
  if (stbi__png_info(s, x, y, comp))
    return 1;
#endif

#ifndef STBI_NO_GIF
  if (stbi__gif_info(s, x, y, comp))
    return 1;
#endif

#ifndef STBI_NO_BMP
  if (stbi__bmp_info(s, x, y, comp))
    return 1;
#endif

#ifndef STBI_NO_PSD
  if (stbi__psd_info(s, x, y, comp))
    return 1;
#endif

#ifndef STBI_NO_PIC
  if (stbi__pic_info(s, x, y, comp))
    return 1;
#endif

#ifndef STBI_NO_PNM
  if (stbi__pnm_info(s, x, y, comp))
    return 1;
#endif

#ifndef STBI_NO_HDR
  if (stbi__hdr_info(s, x, y, comp))
    return 1;
#endif

// test tga last because it's a crappy test!
#ifndef STBI_NO_TGA
  if (stbi__tga_info(s, x, y, comp))
    return 1;
#endif
  return stbi__err("unknown image type",
                   "Image not of any known type, or corrupt");
}

#ifndef STBI_NO_STDIO
STBIDEF int stbi_info(char const *filename, int *x, int *y, int *comp) {
  FILE *f = stbi__fopen(filename, "rb");
  int result;
  if (!f)
    return stbi__err("can't fopen", "Unable to open file");
  result = stbi_info_from_file(f, x, y, comp);
  fclose(f);
  return result;
}

STBIDEF int stbi_info_from_file(FILE *f, int *x, int *y, int *comp) {
  int r;
  stbi__context s;
  long pos = ftell(f);
  stbi__start_file(&s, f);
  r = stbi__info_main(&s, x, y, comp);
  fseek(f, pos, SEEK_SET);
  return r;
}
#endif // !STBI_NO_STDIO

STBIDEF int stbi_info_from_memory(stbi_uc const *buffer, int len, int *x,
                                  int *y, int *comp) {
  stbi__context s;
  stbi__start_mem(&s, buffer, len);
  return stbi__info_main(&s, x, y, comp);
}

STBIDEF int stbi_info_from_callbacks(stbi_io_callbacks const *c, void *user,
                                     int *x, int *y, int *comp) {
  stbi__context s;
  stbi__start_callbacks(&s, (stbi_io_callbacks *)c, user);
  return stbi__info_main(&s, x, y, comp);
}

#endif // STB_IMAGE_IMPLEMENTATION

  /*
     revision history:
        2.16  (2017-07-23) all functions have 16-bit variants;
                           STBI_NO_STDIO works again;
                           compilation fixes;
                           fix rounding in unpremultiply;
                           optimize vertical flip;
                           disable raw_len validation;
                           documentation fixes
        2.15  (2017-03-18) fix png-1,2,4 bug; now all Imagenet JPGs decode;
                           warning fixes; disable run-time SSE detection on gcc;
                           uniform handling of optional "return" values;
                           thread-safe initialization of zlib tables
        2.14  (2017-03-03) remove deprecated STBI_JPEG_OLD; fixes for Imagenet
     JPGs 2.13  (2016-11-29) add 16-bit API, only supported for PNG right now
        2.12  (2016-04-02) fix typo in 2.11 PSD fix that caused crashes
        2.11  (2016-04-02) allocate large structures on the stack
                           remove white matting for transparent PSD
                           fix reported channel count for PNG & BMP
                           re-enable SSE2 in non-gcc 64-bit
                           support RGB-formatted JPEG
                           read 16-bit PNGs (only as 8-bit)
        2.10  (2016-01-22) avoid warning introduced in 2.09 by
     STBI_REALLOC_SIZED 2.09  (2016-01-16) allow comments in PNM files
                           16-bit-per-pixel TGA (not bit-per-component)
                           info() for TGA could break due to .hdr handling
                           info() for BMP to shares code instead of sloppy parse
                           can use STBI_REALLOC_SIZED if allocator doesn't
     support realloc code cleanup 2.08  (2015-09-13) fix to 2.07 cleanup,
     reading RGB PSD as RGBA 2.07  (2015-09-13) fix compiler warnings partial
     animated GIF support limited 16-bpc PSD support #ifdef unused functions bug
     with < 92 byte PIC,PNM,HDR,TGA 2.06  (2015-04-19) fix bug where PSD returns
     wrong '*comp' value 2.05  (2015-04-19) fix bug in progressive JPEG
     handling, fix warning 2.04  (2015-04-15) try to re-enable SIMD on MinGW
     64-bit 2.03  (2015-04-12) extra corruption checking (mmozeiko)
                           stbi_set_flip_vertically_on_load (nguillemot)
                           fix NEON support; fix mingw support
        2.02  (2015-01-19) fix incorrect assert, fix warning
        2.01  (2015-01-17) fix various warnings; suppress SIMD on gcc 32-bit
     without -msse2 2.00b (2014-12-25) fix STBI_MALLOC in progressive JPEG 2.00
     (2014-12-25) optimize JPG, including x86 SSE2 & NEON SIMD (ryg) progressive
     JPEG (stb) PGM/PPM support (Ken Miller) STBI_MALLOC,STBI_REALLOC,STBI_FREE
                           GIF bugfix -- seemingly never worked
                           STBI_NO_*, STBI_ONLY_*
        1.48  (2014-12-14) fix incorrectly-named assert()
        1.47  (2014-12-14) 1/2/4-bit PNG support, both direct and paletted (Omar
     Cornut & stb) optimize PNG (ryg) fix bug in interlaced PNG with
     user-specified channel count (stb) 1.46  (2014-08-26) fix broken tRNS chunk
     (colorkey-style transparency) in non-paletted PNG 1.45  (2014-08-16) fix
     MSVC-ARM internal compiler error by wrapping malloc 1.44  (2014-08-07)
                various warning fixes from Ronny Chevalier
        1.43  (2014-07-15)
                fix MSVC-only compiler problem in code changed in 1.42
        1.42  (2014-07-09)
                don't define _CRT_SECURE_NO_WARNINGS (affects user code)
                fixes to stbi__cleanup_jpeg path
                added STBI_ASSERT to avoid requiring assert.h
        1.41  (2014-06-25)
                fix search&replace from 1.36 that messed up comments/error
     messages 1.40  (2014-06-22) fix gcc struct-initialization warning 1.39
     (2014-06-15) fix to TGA optimization when req_comp != number of components
     in TGA; fix to GIF loading because BMP wasn't rewinding (whoops, no GIFs in
     my test suite) add support for BMP version 5 (more ignored fields) 1.38
     (2014-06-06) suppress MSVC warnings on integer casts truncating values fix
     accidental rename of 'skip' field of I/O 1.37  (2014-06-04) remove
     duplicate typedef 1.36  (2014-06-03) convert to header file single-file
     library if de-iphone isn't set, load iphone images color-swapped instead of
     returning NULL 1.35  (2014-05-27) various warnings fix broken STBI_SIMD
     path fix bug where stbi_load_from_file no longer left file pointer in
     correct place fix broken non-easy path for 32-bit BMP (possibly never used)
                TGA optimization by Arseny Kapoulkine
        1.34  (unknown)
                use STBI_NOTUSED in stbi__resample_row_generic(), fix one more
     leak in tga failure case 1.33  (2011-07-14) make stbi_is_hdr work in
     STBI_NO_HDR (as specified), minor compiler-friendly improvements 1.32
     (2011-07-13) support for "info" function for all supported filetypes
     (SpartanJ) 1.31  (2011-06-20) a few more leak fixes, bug in PNG handling
     (SpartanJ) 1.30  (2011-06-11) added ability to load files via callbacks to
     accomidate custom input streams (Ben Wenger) removed deprecated
     format-specific test/load functions removed support for installable file
     formats (stbi_loader) -- would have been broken for IO callbacks anyway
                error cases in bmp and tga give messages and don't leak (Raymond
     Barbiero, grisha) fix inefficiency in decoding 32-bit BMP (David Woo) 1.29
     (2010-08-16) various warning fixes from Aurelien Pocheville 1.28
     (2010-08-01) fix bug in GIF palette transparency (SpartanJ) 1.27
     (2010-08-01) cast-to-stbi_uc to fix warnings 1.26  (2010-07-24) fix bug in
     file buffering for PNG reported by SpartanJ 1.25  (2010-07-17) refix
     trans_data warning (Won Chun) 1.24  (2010-07-12) perf improvements reading
     from files on platforms with lock-heavy fgetc() minor perf improvements for
     jpeg deprecated type-specific functions so we'll get feedback if they're
     needed attempt to fix trans_data warning (Won Chun) 1.23    fixed bug in
     iPhone support 1.22  (2010-07-10) removed image *writing* support stbi_info
     support from Jetro Lauha GIF support from Jean-Marc Lienher iPhone
     PNG-extensions from James Brown warning-fixes from Nicolas Schulz and Janez
     Zemva (i.stbi__err. Janez (U+017D)emva) 1.21    fix use of 'stbi_uc' in
     header (reported by jon blow) 1.20    added support for Softimage PIC, by
     Tom Seddon 1.19    bug in interlaced PNG corruption check (found by ryg)
        1.18  (2008-08-02)
                fix a threading bug (local mutable static)
        1.17    support interlaced PNG
        1.16    major bugfix - stbi__convert_format converted one too many
     pixels 1.15    initialize some fields for thread safety 1.14    fix
     threadsafe conversion bug header-file-only version (#define
     STBI_HEADER_FILE_ONLY before including) 1.13    threadsafe 1.12    const
     qualifiers in the API 1.11    Support installable IDCT, colorspace
     conversion routines 1.10    Fixes for 64-bit (don't use "unsigned long")
                optimized upsampling by Fabian "ryg" Giesen
        1.09    Fix format-conversion for PSD code (bad global variables!)
        1.08    Thatcher Ulrich's PSD code integrated by Nicolas Schulz
        1.07    attempt to fix C++ warning/errors again
        1.06    attempt to fix C++ warning/errors again
        1.05    fix TGA loading to return correct *comp and use good luminance
     calc 1.04    default float alpha is 1, not 255; use 'void *' for
     stbi_image_free 1.03    bugfixes to STBI_NO_STDIO, STBI_NO_HDR 1.02
     support for (subset of) HDR files, float interface for preferred access to
     them 1.01    fix bug: possible bug in handling right-side up bmps... not
     sure fix bug: the stbi__bmp_load() and stbi__tga_load() functions didn't
     work at all 1.00    interface to zlib that skips zlib header 0.99
     correct handling of alpha in palette 0.98    TGA loader by lonesock;
     dynamically add loaders (untested) 0.97    jpeg errors on too large a file;
     also catch another malloc failure 0.96    fix detection of invalid v value
     - particleman@mollyrocket forum 0.95    during header scan, seek to markers
     in case of padding 0.94    STBI_NO_STDIO to disable stdio usage; rename all
     #defines the same 0.93    handle jpegtran output; verbose errors 0.92
     read 4,8,16,24,32-bit BMP files of several formats 0.91    output 24-bit
     Windows 3.0 BMP files 0.90    fix a few more warnings; bump version number
     to approach 1.0 0.61    bugfixes due to Marc LeBlanc, Christopher Lloyd
        0.60    fix compiling as c++
        0.59    fix warnings: merge Dave Moore's -Wall fixes
        0.58    fix bug: zlib uncompressed mode len/nlen was wrong endian
        0.57    fix bug: jpg last huffman symbol before marker was >9 bits but
     less than 16 available 0.56    fix bug: zlib uncompressed mode len vs. nlen
        0.55    fix bug: restart_interval not initialized to 0
        0.54    allow NULL for 'int *comp'
        0.53    fix bug in png 3->4; speedup png decoding
        0.52    png handles req_comp=3,4 directly; minor cleanup; jpeg comments
        0.51    obey req_comp requests, 1-component jpegs return as 1-component,
                on 'test' only check type, not whether we support this variant
        0.50  (2006-11-19)
                first released version
  */

  /*
  ------------------------------------------------------------------------------
  This software is available under 2 licenses -- choose whichever you prefer.
  ------------------------------------------------------------------------------
  ALTERNATIVE A - MIT License
  Copyright (c) 2017 Sean Barrett
  Permission is hereby granted, free of charge, to any person obtaining a copy
  of this software and associated documentation files (the "Software"), to deal
  in the Software without restriction, including without limitation the rights
  to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
  copies of the Software, and to permit persons to whom the Software is
  furnished to do so, subject to the following conditions: The above copyright
  notice and this permission notice shall be included in all copies or
  substantial portions of the Software. THE SOFTWARE IS PROVIDED "AS IS",
  WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO
  THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
  NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE
  FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT,
  TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR
  THE USE OR OTHER DEALINGS IN THE SOFTWARE.
  ------------------------------------------------------------------------------
  ALTERNATIVE B - Public Domain (www.unlicense.org)
  This is free and unencumbered software released into the public domain.
  Anyone is free to copy, modify, publish, use, compile, sell, or distribute
  this software, either in source code form or as a compiled binary, for any
  purpose, commercial or non-commercial, and by any means. In jurisdictions that
  recognize copyright laws, the author or authors of this software dedicate any
  and all copyright interest in the software to the public domain. We make this
  dedication for the benefit of the public at large and to the detriment of our
  heirs and successors. We intend this dedication to be an overt act of
  relinquishment in perpetuity of all present and future rights to this software
  under copyright law. THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY
  KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
  MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO
  EVENT SHALL THE AUTHORS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY,
  WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR
  IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
  ------------------------------------------------------------------------------
  */
  //------------------------------------------------------------------------------
  // END stb_image.h
  //------------------------------------------------------------------------------

  //------------------------------------------------------------------------------
  // Start gsl_assert
  //------------------------------------------------------------------------------
  ///////////////////////////////////////////////////////////////////////////////
  //
  // Copyright (c) 2015 Microsoft Corporation. All rights reserved.
  //
  // This code is licensed under the MIT License (MIT).
  //
  // THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
  // IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
  // FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
  // AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
  // LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
  // FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER
  // DEALINGS IN THE SOFTWARE.
  //
  ///////////////////////////////////////////////////////////////////////////////

#ifndef GSL_CONTRACTS_H
#define GSL_CONTRACTS_H

#include <exception>
#include <stdexcept> // for logic_error

//
// make suppress attributes parse for some compilers
// Hopefully temporary until suppression standardization occurs
//
#if defined(__clang__)
#define GSL_SUPPRESS(x) [[gsl::suppress("x")]]
#else
#if defined(_MSC_VER)
#define GSL_SUPPRESS(x) [[gsl::suppress(x)]]
#else
#define GSL_SUPPRESS(x)
#endif // _MSC_VER
#endif // __clang__

//
// Temporary until MSVC STL supports no-exceptions mode.
// Currently terminate is a no-op in this mode, so we add termination behavior
// back
//
#if defined(_MSC_VER) && defined(_HAS_EXCEPTIONS) && !_HAS_EXCEPTIONS
#define GSL_MSVC_USE_STL_NOEXCEPTION_WORKAROUND
#include <intrin.h>
#define RANGE_CHECKS_FAILURE 0

#if defined(__clang__)
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Winvalid-noreturn"
#endif

#endif

//
// There are three configuration options for this GSL implementation's behavior
// when pre/post conditions on the GSL types are violated:
//
// 1. GSL_TERMINATE_ON_CONTRACT_VIOLATION: std::terminate will be called
// (default)
// 2. GSL_THROW_ON_CONTRACT_VIOLATION: a gsl::fail_fast exception will be thrown
// 3. GSL_UNENFORCED_ON_CONTRACT_VIOLATION: nothing happens
//
#if !(defined(GSL_THROW_ON_CONTRACT_VIOLATION) ||                              \
      defined(GSL_TERMINATE_ON_CONTRACT_VIOLATION) ||                          \
      defined(GSL_UNENFORCED_ON_CONTRACT_VIOLATION))
#define GSL_TERMINATE_ON_CONTRACT_VIOLATION
#endif

#define GSL_STRINGIFY_DETAIL(x) #x
#define GSL_STRINGIFY(x) GSL_STRINGIFY_DETAIL(x)

#if defined(__clang__) || defined(__GNUC__)
#define GSL_LIKELY(x) __builtin_expect(!!(x), 1)
#define GSL_UNLIKELY(x) __builtin_expect(!!(x), 0)
#else
#define GSL_LIKELY(x) (!!(x))
#define GSL_UNLIKELY(x) (!!(x))
#endif

//
// GSL_ASSUME(cond)
//
// Tell the optimizer that the predicate cond must hold. It is unspecified
// whether or not cond is actually evaluated.
//
#ifdef _MSC_VER
#define GSL_ASSUME(cond) __assume(cond)
#elif defined(__GNUC__)
#define GSL_ASSUME(cond)                                                       \
  ((cond) ? static_cast<void>(0) : __builtin_unreachable())
#else
#define GSL_ASSUME(cond) static_cast<void>((cond) ? 0 : 0)
#endif

//
// GSL.assert: assertions
//

namespace gsl {
struct fail_fast : public std::logic_error {
  explicit fail_fast(char const *const message) : std::logic_error(message) {}
};

namespace details {
#if defined(GSL_MSVC_USE_STL_NOEXCEPTION_WORKAROUND)

typedef void(__cdecl *terminate_handler)();

GSL_SUPPRESS(f .6) // NO-FORMAT: attribute
[[noreturn]] inline void __cdecl default_terminate_handler() {
  __fastfail(RANGE_CHECKS_FAILURE);
}

inline gsl::details::terminate_handler &get_terminate_handler() noexcept {
  static terminate_handler handler = &default_terminate_handler;
  return handler;
}

#endif

[[noreturn]] inline void terminate() noexcept {
#if defined(GSL_MSVC_USE_STL_NOEXCEPTION_WORKAROUND)
  (*gsl::details::get_terminate_handler())();
#else
  std::terminate();
#endif
}

#if defined(GSL_TERMINATE_ON_CONTRACT_VIOLATION)

template <typename Exception>
[[noreturn]] void throw_exception(Exception &&) noexcept {
  gsl::details::terminate();
}

#else

template <typename Exception>
[[noreturn]] void throw_exception(Exception &&exception) {
  throw std::forward<Exception>(exception);
}

#endif // GSL_TERMINATE_ON_CONTRACT_VIOLATION

} // namespace details
} // namespace gsl

#if defined(GSL_THROW_ON_CONTRACT_VIOLATION)

#define GSL_CONTRACT_CHECK(type, cond)                                         \
  (GSL_LIKELY(cond) ? static_cast<void>(0)                                     \
                    : gsl::details::throw_exception(                           \
                          gsl::fail_fast("GSL: " type " failure at " __FILE__  \
                                         ": " GSL_STRINGIFY(__LINE__))))

#elif defined(GSL_TERMINATE_ON_CONTRACT_VIOLATION)

#define GSL_CONTRACT_CHECK(type, cond)                                         \
  (GSL_LIKELY(cond) ? static_cast<void>(0) : gsl::details::terminate())

#elif defined(GSL_UNENFORCED_ON_CONTRACT_VIOLATION)

#define GSL_CONTRACT_CHECK(type, cond) GSL_ASSUME(cond)

#endif // GSL_THROW_ON_CONTRACT_VIOLATION

#define Expects(cond) GSL_CONTRACT_CHECK("Precondition", cond)
#define Ensures(cond) GSL_CONTRACT_CHECK("Postcondition", cond)

#if defined(GSL_MSVC_USE_STL_NOEXCEPTION_WORKAROUND) && defined(__clang__)
#pragma clang diagnostic pop
#endif

#endif // GSL_CONTRACTS_H
//------------------------------------------------------------------------------
// END gsl_assert
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
// Start gsl_byte
//------------------------------------------------------------------------------
///////////////////////////////////////////////////////////////////////////////
//
// Copyright (c) 2015 Microsoft Corporation. All rights reserved.
//
// This code is licensed under the MIT License (MIT).
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
// THE SOFTWARE.
//
///////////////////////////////////////////////////////////////////////////////

#ifndef GSL_BYTE_H
#define GSL_BYTE_H

//
// make suppress attributes work for some compilers
// Hopefully temporary until suppression standardization occurs
//
#if defined(__clang__)
#define GSL_SUPPRESS(x) [[gsl::suppress("x")]]
#else
#if defined(_MSC_VER)
#define GSL_SUPPRESS(x) [[gsl::suppress(x)]]
#else
#define GSL_SUPPRESS(x)
#endif // _MSC_VER
#endif // __clang__

#include <type_traits>

#ifdef _MSC_VER

#pragma warning(push)

// Turn MSVC /analyze rules that generate too much noise. TODO: fix in the tool.
#pragma warning(disable : 26493) // don't use c-style casts // TODO: MSVC
                                 // suppression in templates does not always
                                 // work

#ifndef GSL_USE_STD_BYTE
// this tests if we are under MSVC and the standard lib has std::byte and it is
// enabled
#if defined(_HAS_STD_BYTE) && _HAS_STD_BYTE

#define GSL_USE_STD_BYTE 1

#else // defined(_HAS_STD_BYTE) && _HAS_STD_BYTE

#define GSL_USE_STD_BYTE 0

#endif // defined(_HAS_STD_BYTE) && _HAS_STD_BYTE
#endif // GSL_USE_STD_BYTE

#else // _MSC_VER

#ifndef GSL_USE_STD_BYTE
// this tests if we are under GCC or Clang with enough -std:c++1z power to get
// us std::byte also check if libc++ version is sufficient (> 5.0) or libstc++
// actually contains std::byte
#if defined(__cplusplus) && (__cplusplus >= 201703L) &&                        \
    (defined(__cpp_lib_byte) && (__cpp_lib_byte >= 201603) ||                  \
     defined(_LIBCPP_VERSION) && (_LIBCPP_VERSION >= 5000))

#define GSL_USE_STD_BYTE 1
#include <cstddef>

#else // defined(__cplusplus) && (__cplusplus >= 201703L) &&
      //   (defined(__cpp_lib_byte) && (__cpp_lib_byte >= 201603)  ||
      //    defined(_LIBCPP_VERSION) && (_LIBCPP_VERSION >= 5000))

#define GSL_USE_STD_BYTE 0

#endif // defined(__cplusplus) && (__cplusplus >= 201703L) &&
       //   (defined(__cpp_lib_byte) && (__cpp_lib_byte >= 201603)  ||
       //    defined(_LIBCPP_VERSION) && (_LIBCPP_VERSION >= 5000))
#endif // GSL_USE_STD_BYTE

#endif // _MSC_VER

// Use __may_alias__ attribute on gcc and clang
#if defined __clang__ || (defined(__GNUC__) && __GNUC__ > 5)
#define byte_may_alias __attribute__((__may_alias__))
#else // defined __clang__ || defined __GNUC__
#define byte_may_alias
#endif // defined __clang__ || defined __GNUC__

namespace gsl {
#if GSL_USE_STD_BYTE

using std::byte;
using std::to_integer;

#else // GSL_USE_STD_BYTE

// This is a simple definition for now that allows
// use of byte within span<> to be standards-compliant
enum class byte_may_alias byte : unsigned char {};

template <class IntegerType,
          class = std::enable_if_t<std::is_integral<IntegerType>::value>>
constexpr byte &operator<<=(byte &b, IntegerType shift) noexcept {
  return b = byte(static_cast<unsigned char>(b) << shift);
}

template <class IntegerType,
          class = std::enable_if_t<std::is_integral<IntegerType>::value>>
constexpr byte operator<<(byte b, IntegerType shift) noexcept {
  return byte(static_cast<unsigned char>(b) << shift);
}

template <class IntegerType,
          class = std::enable_if_t<std::is_integral<IntegerType>::value>>
constexpr byte &operator>>=(byte &b, IntegerType shift) noexcept {
  return b = byte(static_cast<unsigned char>(b) >> shift);
}

template <class IntegerType,
          class = std::enable_if_t<std::is_integral<IntegerType>::value>>
constexpr byte operator>>(byte b, IntegerType shift) noexcept {
  return byte(static_cast<unsigned char>(b) >> shift);
}

constexpr byte &operator|=(byte &l, byte r) noexcept {
  return l = byte(static_cast<unsigned char>(l) |
                  static_cast<unsigned char>(r));
}

constexpr byte operator|(byte l, byte r) noexcept {
  return byte(static_cast<unsigned char>(l) | static_cast<unsigned char>(r));
}

constexpr byte &operator&=(byte &l, byte r) noexcept {
  return l = byte(static_cast<unsigned char>(l) &
                  static_cast<unsigned char>(r));
}

constexpr byte operator&(byte l, byte r) noexcept {
  return byte(static_cast<unsigned char>(l) & static_cast<unsigned char>(r));
}

constexpr byte &operator^=(byte &l, byte r) noexcept {
  return l = byte(static_cast<unsigned char>(l) ^
                  static_cast<unsigned char>(r));
}

constexpr byte operator^(byte l, byte r) noexcept {
  return byte(static_cast<unsigned char>(l) ^ static_cast<unsigned char>(r));
}

constexpr byte operator~(byte b) noexcept {
  return byte(~static_cast<unsigned char>(b));
}

template <class IntegerType,
          class = std::enable_if_t<std::is_integral<IntegerType>::value>>
constexpr IntegerType to_integer(byte b) noexcept {
  return static_cast<IntegerType>(b);
}

#endif // GSL_USE_STD_BYTE

template <bool E, typename T> constexpr byte to_byte_impl(T t) noexcept {
  static_assert(E, "gsl::to_byte(t) must be provided an unsigned char, "
                   "otherwise data loss may occur. "
                   "If you are calling to_byte with an integer contant use: "
                   "gsl::to_byte<t>() version.");
  return static_cast<byte>(t);
}
template <>
// NOTE: need suppression since c++14 does not allow "return {t}"
// GSL_SUPPRESS(type.4) // NO-FORMAT: attribute // TODO: suppression does not
// work
constexpr byte to_byte_impl<true, unsigned char>(unsigned char t) noexcept {
  return byte(t);
}

template <typename T> constexpr byte to_byte(T t) noexcept {
  return to_byte_impl<std::is_same<T, unsigned char>::value, T>(t);
}

template <int I> constexpr byte to_byte() noexcept {
  static_assert(
      I >= 0 && I <= 255,
      "gsl::byte only has 8 bits of storage, values must be in range 0-255");
  return static_cast<byte>(I);
}

} // namespace gsl

#ifdef _MSC_VER
#pragma warning(pop)
#endif // _MSC_VER

#endif // GSL_BYTE_H
//------------------------------------------------------------------------------
// END gsl_byte
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
// Start types.h
//------------------------------------------------------------------------------

#include <glm/glm.hpp>

namespace givr {

using vec2f = glm::vec2;
using vec3f = glm::vec3;
using vec4f = glm::vec4;
using mat4f = glm::mat4;
using mat3f = glm::mat3;
using mat2f = glm::mat2;
using mat2x3f = glm::mat2x3;
using mat3x2f = glm::mat3x2;
using mat4x3f = glm::mat4x3;
using mat3x4f = glm::mat3x4;

enum class PrimitiveType {
  POINTS,
  LINES,
  LINE_LOOP,
  LINE_STRIP,
  TRIANGLES,
  TRIANGLE_STRIP,
  TRIANGLE_FAN,
  LINES_ADJACENCY,
  LINE_STRIP_ADJACENCY,
  TRIANGLES_ADJACENCY,
  TRIANGLE_STRIP_ADJACENCY
};

GLenum getMode(PrimitiveType const &t);

enum class BufferUsageType { STATIC_DRAW, DYNAMIC_DRAW };
GLenum getBufferUsageType(BufferUsageType const &d);

}; // end namespace givr
//------------------------------------------------------------------------------
// END types.h
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
// Start shader.h
//------------------------------------------------------------------------------

#include <fstream>
#include <iostream>
#include <sstream>
#include <string>

namespace givr {

class Shader {
public:
  Shader(const std::string &source, GLenum shaderType);
  // TODO(lw): make a version that just receives the source directly.
  ~Shader();

  // Default ctor/dtor & move operations
  Shader(Shader &&other) = default;
  Shader &operator=(Shader &&rhs) = default;

  // But no copy or assignment. Bad.
  Shader(const Shader &) = delete;
  Shader &operator=(const Shader &) = delete;

  operator GLuint() const { return m_shaderID; }

private:
  GLuint m_shaderID = 0;
};
}; // end namespace givr
//------------------------------------------------------------------------------
// END shader.h
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
// Start texture.h
//------------------------------------------------------------------------------

#include <array>
#include <vector>

namespace givr {
class Texture {
public:
  Texture();
  // TODO(lw): make a version that just receives the source directly.
  ~Texture();

  operator GLuint() const { return m_textureID; }
  void alloc();
  void dealloc();
  void bind(GLenum target);
  void load(GLenum target, std::string filename, GLint level = 0);

private:
  GLuint m_textureID = 0;
};
}; // end namespace givr
//------------------------------------------------------------------------------
// END texture.h
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
// Start vertex_array.h
//------------------------------------------------------------------------------

namespace givr {
class VertexArray {
public:
  VertexArray();
  // TODO(lw): make a version that just receives the source directly.
  ~VertexArray();

  operator GLuint() const { return m_vertexArrayID; }
  void alloc();
  void dealloc();
  void bind();
  void unbind();

private:
  GLuint m_vertexArrayID = 0;
};
}; // end namespace givr
//------------------------------------------------------------------------------
// END vertex_array.h
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
// Start gsl_util
//------------------------------------------------------------------------------
///////////////////////////////////////////////////////////////////////////////
//
// Copyright (c) 2015 Microsoft Corporation. All rights reserved.
//
// This code is licensed under the MIT License (MIT).
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
// THE SOFTWARE.
//
///////////////////////////////////////////////////////////////////////////////

#ifndef GSL_UTIL_H
#define GSL_UTIL_H

#include <array>
#include <cstddef>          // for ptrdiff_t, size_t
#include <exception>        // for exception
#include <initializer_list> // for initializer_list
#include <type_traits>      // for is_signed, integral_constant
#include <utility>          // for forward

#if defined(_MSC_VER) && !defined(__clang__)

#pragma warning(push)
#pragma warning(disable : 4127) // conditional expression is constant

#if _MSC_VER < 1910
#pragma push_macro("constexpr")
#define constexpr /*constexpr*/
#endif            // _MSC_VER < 1910
#endif            // _MSC_VER

#if (defined(_MSC_VER) && _MSC_VER < 1910) ||                                  \
    (!defined(__clang__) && defined(__GNUC__) && __GUNC__ < 6)
#define GSL_CONSTEXPR_NARROW 0
#else
#define GSL_CONSTEXPR_NARROW 1
#endif

namespace gsl {
//
// GSL.util: utilities
//

// index type for all container indexes/subscripts/sizes
using index = std::ptrdiff_t;

// final_action allows you to ensure something gets run at the end of a scope
template <class F> class final_action {
public:
  explicit final_action(F f) noexcept : f_(std::move(f)) {}

  final_action(final_action &&other) noexcept
      : f_(std::move(other.f_)), invoke_(other.invoke_) {
    other.invoke_ = false;
  }

  final_action(const final_action &) = delete;
  final_action &operator=(const final_action &) = delete;
  final_action &operator=(final_action &&) = delete;

  GSL_SUPPRESS(f .6) // NO-FORMAT: attribute // terminate if throws
  ~final_action() noexcept {
    if (invoke_)
      f_();
  }

private:
  F f_;
  bool invoke_{true};
};

// finally() - convenience function to generate a final_action
template <class F> final_action<F> finally(const F &f) noexcept {
  return final_action<F>(f);
}

template <class F> final_action<F> finally(F &&f) noexcept {
  return final_action<F>(std::forward<F>(f));
}

// narrow_cast(): a searchable way to do narrowing casts of values
template <class T, class U>
GSL_SUPPRESS(type .1) // NO-FORMAT: attribute
constexpr T narrow_cast(U &&u) noexcept {
  return static_cast<T>(std::forward<U>(u));
}

struct narrowing_error : public std::exception {};

namespace details {
template <class T, class U>
struct is_same_signedness
    : public std::integral_constant<bool, std::is_signed<T>::value ==
                                              std::is_signed<U>::value> {};
} // namespace details

// narrow() : a checked version of narrow_cast() that throws if the cast changed
// the value
template <class T, class U>
GSL_SUPPRESS(type .1) // NO-FORMAT: attribute
GSL_SUPPRESS(f .6)    // NO-FORMAT: attribute // TODO: MSVC /analyze does not
                      // recognise noexcept(false)
#if GSL_CONSTEXPR_NARROW
    constexpr
#endif
    T narrow(U u) noexcept(false) {
  T t = narrow_cast<T>(u);
  if (static_cast<U>(t) != u)
    gsl::details::throw_exception(narrowing_error());
  if (!details::is_same_signedness<T, U>::value && ((t < T{}) != (u < U{})))
    gsl::details::throw_exception(narrowing_error());
  return t;
}

//
// at() - Bounds-checked way of accessing builtin arrays, std::array,
// std::vector
//
template <class T, std::size_t N>
GSL_SUPPRESS(bounds .4) // NO-FORMAT: attribute
GSL_SUPPRESS(bounds .2) // NO-FORMAT: attribute
    constexpr T &at(T (&arr)[N], const index i) {
  Expects(i >= 0 && i < narrow_cast<index>(N));
  return arr[narrow_cast<std::size_t>(i)];
}

template <class Cont>
GSL_SUPPRESS(bounds .4) // NO-FORMAT: attribute
GSL_SUPPRESS(bounds .2) // NO-FORMAT: attribute
    constexpr auto at(Cont &cont, const index i)
        -> decltype(cont[cont.size()]) {
  Expects(i >= 0 && i < narrow_cast<index>(cont.size()));
  using size_type = decltype(cont.size());
  return cont[narrow_cast<size_type>(i)];
}

template <class T>
GSL_SUPPRESS(bounds .1) // NO-FORMAT: attribute
constexpr T at(const std::initializer_list<T> cont, const index i) {
  Expects(i >= 0 && i < narrow_cast<index>(cont.size()));
  return *(cont.begin() + i);
}

} // namespace gsl

#if defined(_MSC_VER) && !defined(__clang__)
#if _MSC_VER < 1910
#undef constexpr
#pragma pop_macro("constexpr")

#endif // _MSC_VER < 1910

#pragma warning(pop)

#endif // _MSC_VER

#endif // GSL_UTIL_H
//------------------------------------------------------------------------------
// END gsl_util
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
// Start pointers
//------------------------------------------------------------------------------
///////////////////////////////////////////////////////////////////////////////
//
// Copyright (c) 2015 Microsoft Corporation. All rights reserved.
//
// This code is licensed under the MIT License (MIT).
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
// THE SOFTWARE.
//
///////////////////////////////////////////////////////////////////////////////

#ifndef GSL_POINTERS_H
#define GSL_POINTERS_H

#include <algorithm>    // for forward
#include <iosfwd>       // for ptrdiff_t, nullptr_t, ostream, size_t
#include <memory>       // for shared_ptr, unique_ptr
#include <system_error> // for hash
#include <type_traits>  // for enable_if_t, is_convertible, is_assignable

#if defined(_MSC_VER) && _MSC_VER < 1910 && !defined(__clang__)
#pragma push_macro("constexpr")
#define constexpr /*constexpr*/

#endif // defined(_MSC_VER) && _MSC_VER < 1910

namespace gsl {

//
// GSL.owner: ownership pointers
//
using std::shared_ptr;
using std::unique_ptr;

//
// owner
//
// owner<T> is designed as a bridge for code that must deal directly with owning
// pointers for some reason
//
// T must be a pointer type
// - disallow construction from any type other than pointer type
//
template <class T, class = std::enable_if_t<std::is_pointer<T>::value>>
using owner = T;

//
// not_null
//
// Restricts a pointer or smart pointer to only hold non-null values.
//
// Has zero size overhead over T.
//
// If T is a pointer (i.e. T == U*) then
// - allow construction from U*
// - disallow construction from nullptr_t
// - disallow default construction
// - ensure construction from null U* fails
// - allow implicit conversion to U*
//
template <class T> class not_null {
public:
  static_assert(std::is_assignable<T &, std::nullptr_t>::value,
                "T cannot be assigned nullptr.");

  template <typename U,
            typename = std::enable_if_t<std::is_convertible<U, T>::value>>
  constexpr not_null(U &&u) : ptr_(std::forward<U>(u)) {
    Expects(ptr_ != nullptr);
  }

  template <
      typename = std::enable_if_t<!std::is_same<std::nullptr_t, T>::value>>
  constexpr not_null(T u) : ptr_(u) {
    Expects(ptr_ != nullptr);
  }

  template <typename U,
            typename = std::enable_if_t<std::is_convertible<U, T>::value>>
  constexpr not_null(const not_null<U> &other) : not_null(other.get()) {}

  not_null(not_null &&other) = default;
  not_null(const not_null &other) = default;
  not_null &operator=(const not_null &other) = default;

  constexpr T get() const {
    Ensures(ptr_ != nullptr);
    return ptr_;
  }

  constexpr operator T() const { return get(); }
  constexpr T operator->() const { return get(); }
  constexpr decltype(auto) operator*() const { return *get(); }

  // prevents compilation when someone attempts to assign a null pointer
  // constant
  not_null(std::nullptr_t) = delete;
  not_null &operator=(std::nullptr_t) = delete;

  // unwanted operators...pointers only point to single objects!
  not_null &operator++() = delete;
  not_null &operator--() = delete;
  not_null operator++(int) = delete;
  not_null operator--(int) = delete;
  not_null &operator+=(std::ptrdiff_t) = delete;
  not_null &operator-=(std::ptrdiff_t) = delete;
  void operator[](std::ptrdiff_t) const = delete;

private:
  T ptr_;
};

template <class T> auto make_not_null(T &&t) {
  return not_null<std::remove_cv_t<std::remove_reference_t<T>>>{
      std::forward<T>(t)};
}

template <class T>
std::ostream &operator<<(std::ostream &os, const not_null<T> &val) {
  os << val.get();
  return os;
}

template <class T, class U>
auto operator==(const not_null<T> &lhs, const not_null<U> &rhs)
    -> decltype(lhs.get() == rhs.get()) {
  return lhs.get() == rhs.get();
}

template <class T, class U>
auto operator!=(const not_null<T> &lhs, const not_null<U> &rhs)
    -> decltype(lhs.get() != rhs.get()) {
  return lhs.get() != rhs.get();
}

template <class T, class U>
auto operator<(const not_null<T> &lhs, const not_null<U> &rhs)
    -> decltype(lhs.get() < rhs.get()) {
  return lhs.get() < rhs.get();
}

template <class T, class U>
auto operator<=(const not_null<T> &lhs, const not_null<U> &rhs)
    -> decltype(lhs.get() <= rhs.get()) {
  return lhs.get() <= rhs.get();
}

template <class T, class U>
auto operator>(const not_null<T> &lhs, const not_null<U> &rhs)
    -> decltype(lhs.get() > rhs.get()) {
  return lhs.get() > rhs.get();
}

template <class T, class U>
auto operator>=(const not_null<T> &lhs, const not_null<U> &rhs)
    -> decltype(lhs.get() >= rhs.get()) {
  return lhs.get() >= rhs.get();
}

// more unwanted operators
template <class T, class U>
std::ptrdiff_t operator-(const not_null<T> &, const not_null<U> &) = delete;
template <class T>
not_null<T> operator-(const not_null<T> &, std::ptrdiff_t) = delete;
template <class T>
not_null<T> operator+(const not_null<T> &, std::ptrdiff_t) = delete;
template <class T>
not_null<T> operator+(std::ptrdiff_t, const not_null<T> &) = delete;

} // namespace gsl

namespace std {
template <class T> struct hash<gsl::not_null<T>> {
  std::size_t operator()(const gsl::not_null<T> &value) const {
    return hash<T>{}(value);
  }
};

} // namespace std

namespace gsl {

//
// strict_not_null
//
// Restricts a pointer or smart pointer to only hold non-null values,
//
// - provides a strict (i.e. explicit contructor from T) wrapper of not_null
// - to be used for new code that wishes the design to be cleaner and make
// not_null
//   checks intentional, or in old code that would like to make the transition.
//
//   To make the transition from not_null, incrementally replace not_null
//   by strict_not_null and fix compilation errors
//
//   Expect to
//   - remove all unneded conversions from raw pointer to not_null and back
//   - make API clear by specifyning not_null in parameters where needed
//   - remove unnesessary asserts
//
template <class T> class strict_not_null : public not_null<T> {
public:
  template <typename U,
            typename = std::enable_if_t<std::is_convertible<U, T>::value>>
  constexpr explicit strict_not_null(U &&u) : not_null<T>(std::forward<U>(u)) {}

  template <
      typename = std::enable_if_t<!std::is_same<std::nullptr_t, T>::value>>
  constexpr explicit strict_not_null(T u) : not_null<T>(u) {}

  template <typename U,
            typename = std::enable_if_t<std::is_convertible<U, T>::value>>
  constexpr strict_not_null(const not_null<U> &other) : not_null<T>(other) {}

  template <typename U,
            typename = std::enable_if_t<std::is_convertible<U, T>::value>>
  constexpr strict_not_null(const strict_not_null<U> &other)
      : not_null<T>(other) {}

  strict_not_null(strict_not_null &&other) = default;
  strict_not_null(const strict_not_null &other) = default;
  strict_not_null &operator=(const strict_not_null &other) = default;
  strict_not_null &operator=(const not_null<T> &other) {
    not_null<T>::operator=(other);
    return *this;
  }

  // prevents compilation when someone attempts to assign a null pointer
  // constant
  strict_not_null(std::nullptr_t) = delete;
  strict_not_null &operator=(std::nullptr_t) = delete;

  // unwanted operators...pointers only point to single objects!
  strict_not_null &operator++() = delete;
  strict_not_null &operator--() = delete;
  strict_not_null operator++(int) = delete;
  strict_not_null operator--(int) = delete;
  strict_not_null &operator+=(std::ptrdiff_t) = delete;
  strict_not_null &operator-=(std::ptrdiff_t) = delete;
  void operator[](std::ptrdiff_t) const = delete;
};

// more unwanted operators
template <class T, class U>
std::ptrdiff_t operator-(const strict_not_null<T> &,
                         const strict_not_null<U> &) = delete;
template <class T>
strict_not_null<T> operator-(const strict_not_null<T> &,
                             std::ptrdiff_t) = delete;
template <class T>
strict_not_null<T> operator+(const strict_not_null<T> &,
                             std::ptrdiff_t) = delete;
template <class T>
strict_not_null<T> operator+(std::ptrdiff_t,
                             const strict_not_null<T> &) = delete;

template <class T> auto make_strict_not_null(T &&t) {
  return strict_not_null<std::remove_cv_t<std::remove_reference_t<T>>>{
      std::forward<T>(t)};
}

} // namespace gsl

namespace std {
template <class T> struct hash<gsl::strict_not_null<T>> {
  std::size_t operator()(const gsl::strict_not_null<T> &value) const {
    return hash<T>{}(value);
  }
};

} // namespace std

#if defined(_MSC_VER) && _MSC_VER < 1910 && !defined(__clang__)

#undef constexpr
#pragma pop_macro("constexpr")

#endif // defined(_MSC_VER) && _MSC_VER < 1910 && !defined(__clang__)

#endif // GSL_POINTERS_H
//------------------------------------------------------------------------------
// END pointers
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
// Start vertex_array_data.h
//------------------------------------------------------------------------------

// This class is used for compile time checking that the data
// is compatible with the style.
namespace givr {
template <givr::PrimitiveType PrimitiveValue> class VertexArrayData {};

// A constexpr function for determining the primitive type from
// the geometry type.
template <typename GeometryT> constexpr givr::PrimitiveType getPrimitive() {
  typedef givr::PrimitiveType pt;
  typedef typename GeometryT::Data Geometry;
  if constexpr (std::is_base_of<VertexArrayData<pt::POINTS>, Geometry>::value) {
    return PrimitiveType::POINTS;
  } else if constexpr (std::is_base_of<VertexArrayData<pt::LINES>,
                                       Geometry>::value) {
    return PrimitiveType::LINES;
  } else if constexpr (std::is_base_of<VertexArrayData<pt::LINE_LOOP>,
                                       Geometry>::value) {
    return PrimitiveType::LINE_LOOP;
  } else if constexpr (std::is_base_of<VertexArrayData<pt::LINE_STRIP>,
                                       Geometry>::value) {
    return PrimitiveType::LINE_STRIP;
  } else if constexpr (std::is_base_of<VertexArrayData<pt::TRIANGLES>,
                                       Geometry>::value) {
    return PrimitiveType::TRIANGLES;
  } else if constexpr (std::is_base_of<VertexArrayData<pt::TRIANGLE_STRIP>,
                                       Geometry>::value) {
    return PrimitiveType::TRIANGLE_STRIP;
  } else if constexpr (std::is_base_of<VertexArrayData<pt::TRIANGLE_FAN>,
                                       Geometry>::value) {
    return PrimitiveType::TRIANGLE_FAN;
  } else if constexpr (std::is_base_of<VertexArrayData<pt::LINES_ADJACENCY>,
                                       Geometry>::value) {
    return PrimitiveType::LINES_ADJACENCY;
  } else if constexpr (std::is_base_of<
                           VertexArrayData<pt::LINE_STRIP_ADJACENCY>,
                           Geometry>::value) {
    return PrimitiveType::LINE_STRIP_ADJACENCY;
  } else if constexpr (std::is_base_of<VertexArrayData<pt::TRIANGLES_ADJACENCY>,
                                       Geometry>::value) {
    return PrimitiveType::TRIANGLES_ADJACENCY;
  } else if constexpr (std::is_base_of<
                           VertexArrayData<pt::TRIANGLE_STRIP_ADJACENCY>,
                           Geometry>::value) {
    return PrimitiveType::TRIANGLE_STRIP_ADJACENCY;
  } else {
    // We can guarantee we can render this.
    // TODO: add a compile time assert here
    return PrimitiveType::POINTS;
  }
}

// A constexpr function which determines if the geometry type is line based
template <typename GeometryT> constexpr bool isLineBased() {
  typedef givr::PrimitiveType pt;
  typedef typename GeometryT::Data Geometry;
  if constexpr (std::is_base_of<VertexArrayData<pt::LINES>, Geometry>::value) {
    return true;
  } else if constexpr (std::is_base_of<VertexArrayData<pt::LINE_LOOP>,
                                       Geometry>::value) {
    return true;
  } else if constexpr (std::is_base_of<VertexArrayData<pt::LINE_STRIP>,
                                       Geometry>::value) {
    return true;
  } else if constexpr (std::is_base_of<VertexArrayData<pt::LINES_ADJACENCY>,
                                       Geometry>::value) {
    return true;
  } else if constexpr (std::is_base_of<
                           VertexArrayData<pt::LINE_STRIP_ADJACENCY>,
                           Geometry>::value) {
    return true;
  } else {
    return false;
  }
}

// A constexpr function which determines if the geometry type is triangle based
template <typename GeometryT> constexpr bool isTriangleBased() {
  typedef givr::PrimitiveType pt;
  typedef typename GeometryT::Data Geometry;
  if constexpr (std::is_base_of<VertexArrayData<pt::TRIANGLES>,
                                Geometry>::value) {
    return true;
  } else if constexpr (std::is_base_of<VertexArrayData<pt::TRIANGLE_STRIP>,
                                       Geometry>::value) {
    return true;
  } else if constexpr (std::is_base_of<VertexArrayData<pt::TRIANGLE_FAN>,
                                       Geometry>::value) {
    return true;
  } else if constexpr (std::is_base_of<VertexArrayData<pt::TRIANGLES_ADJACENCY>,
                                       Geometry>::value) {
    return true;
  } else if constexpr (std::is_base_of<
                           VertexArrayData<pt::TRIANGLE_STRIP_ADJACENCY>,
                           Geometry>::value) {
    return true;
  } else {
    return false;
  }
}

// A constexpr function which determines if the geometry type is point based
template <typename GeometryT> constexpr bool isPointBased() {
  typedef givr::PrimitiveType pt;
  typedef typename GeometryT::Data Geometry;
  if constexpr (std::is_base_of<VertexArrayData<pt::POINTS>, Geometry>::value) {
    return true;
  } else {
    return false;
  }
}
}; // namespace givr
//------------------------------------------------------------------------------
// END vertex_array_data.h
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
// Start parameters.h
//------------------------------------------------------------------------------
#include <string>

//------------------------------------------------------------------------------
// Defines all of the parameters to camera functions.
//
// Uses custom types for each name so that we can pass them in as "named"
// parameters.
//------------------------------------------------------------------------------

namespace givr {
namespace camera {

using FieldOfViewY = utility::Type<float, struct FieldOfViewY_Tag>;
using AspectRatio = utility::Type<float, struct AspectRatio_Tag>;
using NearDistance = utility::Type<float, struct NearDistance_Tag>;
using FarDistance = utility::Type<float, struct FarDistance_Tag>;
using Latitude = utility::Type<float, struct Latitude_Tag>;
using Longitude = utility::Type<float, struct Longitude_Tag>;
using Zoom = utility::Type<float, struct Zoom_Tag>;
using Translation = utility::Type<vec3f, struct Translation_Tag>;
using Left = utility::Type<float, struct Left_Tag>;
using Right = utility::Type<float, struct Right_Tag>;
using Bottom = utility::Type<float, struct Bottom_Tag>;
using Top = utility::Type<float, struct Top_Tag>;

} // namespace camera
} // end namespace givr
//------------------------------------------------------------------------------
// END parameters.h
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
// Start projection.h
//------------------------------------------------------------------------------

namespace givr {
namespace camera {
template <typename... Types>
struct Projection : public utility::TypedStruct<Types...> {};
} // namespace camera
} // end namespace givr
//------------------------------------------------------------------------------
// END projection.h
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
// Start camera.h
//------------------------------------------------------------------------------

namespace givr {
namespace camera {
template <typename... Types>
struct Camera : public utility::TypedStruct<Types...> {};
} // namespace camera
} // end namespace givr
//------------------------------------------------------------------------------
// END camera.h
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
// Start view_context.h
//------------------------------------------------------------------------------

namespace givr {
namespace camera {
template <typename CameraT, typename ProjectionT> struct ViewContext {
  CameraT camera;
  ProjectionT projection;
};

template <typename CameraT, typename ProjectionT>
ViewContext<CameraT, ProjectionT> View(CameraT const &camera,
                                       ProjectionT const &projection) {
  return ViewContext<CameraT, ProjectionT>{camera, projection};
}
} // end namespace camera
} // end namespace givr
//------------------------------------------------------------------------------
// END view_context.h
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
// Start parameters.h
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
// Defines all of the parameters to style functions.
//
// Uses custom types for each name so that we can pass them in as "named"
// parameters.
//------------------------------------------------------------------------------

namespace givr {
namespace style {

using AmbientFactor = utility::Type<float, struct AmbientFactor_Tag>;
using Colour = utility::Type<vec3f, struct Colour_Tag>;
using LightPosition = utility::Type<vec3f, struct LightPosition_Tag>;
using PerVertexColour = utility::Type<bool, struct PerVertexColor_Tag>;
using ShowWireFrame = utility::Type<bool, struct ShowWireFrame_Tag>;
using WireFrameColour = utility::Type<vec3f, struct WireFrameColour_Tag>;
using WireFrameWidth = utility::Type<float, struct WireFrameWidth_Tag>;
using PhongExponent = utility::Type<float, struct PhongExponent_Tag>;
using SpecularFactor = utility::Type<float, struct SpecularFactor_Tag>;
using Width = utility::Type<float, struct Width_Tag>;
using GenerateNormals = utility::Type<bool, struct GenerateNormals_Tag>;
using ColorTexture = utility::Type<Texture, struct ColorTexture_Tag>;

} // end namespace style
} // end namespace givr
//------------------------------------------------------------------------------
// END parameters.h
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
// Start style.h
//------------------------------------------------------------------------------

namespace givr {
namespace style {
template <typename... Types>
struct Style : public utility::TypedStruct<Types...> {};
} // end namespace style
} // end namespace givr
//------------------------------------------------------------------------------
// END style.h
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
// Start parameters.h
//------------------------------------------------------------------------------
#include <string>

//------------------------------------------------------------------------------
// Defines all of the parameters to geometry functions.
//
// Uses custom types for each name so that we can pass them in as "named"
// parameters.
//------------------------------------------------------------------------------

namespace givr {
namespace geometry {

using Point = utility::Type<vec3f, struct Point_Tag>;
using Point1 = utility::Type<vec3f, struct Point1_Tag>;
using Point2 = utility::Type<vec3f, struct Point2_Tag>;
using Point3 = utility::Type<vec3f, struct Point3_Tag>;
using Point4 = utility::Type<vec3f, struct Point4_Tag>;
using Centroid = utility::Type<vec3f, struct Centroid_Tag>;
using Radius = utility::Type<float, struct Point3_Tag>;
using AzimuthPoints = utility::Type<std::size_t, struct AzimuthPoints_Tag>;
using AltitudePoints = utility::Type<std::size_t, struct AltitudePoints_Tag>;
using Filename = utility::Type<std::string, struct Point3_Tag>;

} // end namespace geometry
} // end namespace givr
//------------------------------------------------------------------------------
// END parameters.h
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
// Start geometry.h
//------------------------------------------------------------------------------

namespace givr {
namespace geometry {
template <typename... Types>
struct Geometry : public utility::TypedStruct<Types...> {};
} // end namespace geometry
} // end namespace givr
//------------------------------------------------------------------------------
// END geometry.h
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
// Start program.h
//------------------------------------------------------------------------------

#include <memory>

namespace givr {

class Program {
public:
  Program(GLuint vertex, GLuint fragment);
  Program(GLuint vertex, GLuint geometry, GLuint fragment);
  ~Program();

  // Default ctor/dtor & move operations
  Program(Program &&other) = default;
  Program &operator=(Program &&rhs) = default;

  // But no copy or assignment. Bad.
  Program(const Program &) = delete;
  Program &operator=(const Program &) = delete;

  operator GLuint() const { return m_programID; }
  void use();

  void setVec2(const std::string &name, vec2f const &value) const;
  void setVec3(const std::string &name, vec3f const &value) const;
  void setMat4(const std::string &name, mat4f const &mat) const;
  void setBool(const std::string &name, bool value) const;
  void setFloat(const std::string &name, float value) const;
  void setInt(const std::string &name, int value) const;

  // TODO: make these work for our math library
  /*
  void setInt(const std::string &name, int value) const;
  void setVec2(const std::string &name, const glm::vec2 &value) const;
  void setVec2(const std::string &name, float x, float y) const;
  void setVec4(const std::string &name, const glm::vec4 &value) const;
  void setVec4(const std::string &name, float x, float y, float z, float w);
  void setMat2(const std::string &name, const glm::mat2 &mat) const;
  void setMat3(const std::string &name, const glm::mat3 &mat) const;*/

private:
  void linkAndErrorCheck();
  GLuint m_programID = 0;
};
}; // end namespace givr
//------------------------------------------------------------------------------
// END program.h
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
// Start span
//------------------------------------------------------------------------------
///////////////////////////////////////////////////////////////////////////////
//
// Copyright (c) 2015 Microsoft Corporation. All rights reserved.
//
// This code is licensed under the MIT License (MIT).
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
// THE SOFTWARE.
//
///////////////////////////////////////////////////////////////////////////////

#ifndef GSL_SPAN_H
#define GSL_SPAN_H

#include <algorithm> // for lexicographical_compare
#include <array>     // for array
#include <cstddef>   // for ptrdiff_t, size_t, nullptr_t
#include <iterator>  // for reverse_iterator, distance, random_access_...
#include <limits>
#include <memory> // for std::addressof
#include <stdexcept>
#include <type_traits> // for enable_if_t, declval, is_convertible, inte...
#include <utility>

#if defined(_MSC_VER) && !defined(__clang__)
#pragma warning(push)

// turn off some warnings that are noisy about our Expects statements
#pragma warning(disable : 4127) // conditional expression is constant
#pragma warning(disable : 4702) // unreachable code

// Turn MSVC /analyze rules that generate too much noise. TODO: fix in the tool.
#pragma warning(                                                               \
    disable : 26495) // uninitalized member when constructor calls constructor
#pragma warning(                                                               \
    disable : 26446) // parser bug does not allow attributes on some templates

#if _MSC_VER < 1910
#pragma push_macro("constexpr")
#define constexpr /*constexpr*/
#define GSL_USE_STATIC_CONSTEXPR_WORKAROUND

#endif // _MSC_VER < 1910
#endif // _MSC_VER

// See if we have enough C++17 power to use a static constexpr data member
// without needing an out-of-line definition
#if !(defined(__cplusplus) && (__cplusplus >= 201703L))
#define GSL_USE_STATIC_CONSTEXPR_WORKAROUND
#endif // !(defined(__cplusplus) && (__cplusplus >= 201703L))

// GCC 7 does not like the signed unsigned missmatch (size_t ptrdiff_t)
// While there is a conversion from signed to unsigned, it happens at
// compiletime, so the compiler wouldn't have to warn indiscriminently, but
// could check if the source value actually doesn't fit into the target type
// and only warn in those cases.
#if defined(__GNUC__) && __GNUC__ > 6
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wsign-conversion"
#endif

namespace gsl {

// [views.constants], constants
constexpr const std::ptrdiff_t dynamic_extent = -1;

template <class ElementType, std::ptrdiff_t Extent = dynamic_extent> class span;

// implementation details
namespace details {
template <class T> struct is_span_oracle : std::false_type {};

template <class ElementType, std::ptrdiff_t Extent>
struct is_span_oracle<gsl::span<ElementType, Extent>> : std::true_type {};

template <class T>
struct is_span : public is_span_oracle<std::remove_cv_t<T>> {};

template <class T> struct is_std_array_oracle : std::false_type {};

template <class ElementType, std::size_t Extent>
struct is_std_array_oracle<std::array<ElementType, Extent>> : std::true_type {};

template <class T>
struct is_std_array : public is_std_array_oracle<std::remove_cv_t<T>> {};

template <std::ptrdiff_t From, std::ptrdiff_t To>
struct is_allowed_extent_conversion
    : public std::integral_constant<bool, From == To ||
                                              From == gsl::dynamic_extent ||
                                              To == gsl::dynamic_extent> {};

template <class From, class To>
struct is_allowed_element_type_conversion
    : public std::integral_constant<
          bool, std::is_convertible<From (*)[], To (*)[]>::value> {};

template <class Span, bool IsConst> class span_iterator {
  using element_type_ = typename Span::element_type;

public:
#ifdef _MSC_VER
  // Tell Microsoft standard library that span_iterators are checked.
  using _Unchecked_type = typename Span::pointer;
#endif

  using iterator_category = std::random_access_iterator_tag;
  using value_type = std::remove_cv_t<element_type_>;
  using difference_type = typename Span::index_type;

  using reference =
      std::conditional_t<IsConst, const element_type_, element_type_> &;
  using pointer = std::add_pointer_t<reference>;

  span_iterator() = default;

  constexpr span_iterator(const Span *span,
                          typename Span::index_type idx) noexcept
      : span_(span), index_(idx) {}

  friend span_iterator<Span, true>;
  template <bool B, std::enable_if_t<!B && IsConst> * = nullptr>
  constexpr span_iterator(const span_iterator<Span, B> &other) noexcept
      : span_iterator(other.span_, other.index_) {}

  GSL_SUPPRESS(bounds .1) // NO-FORMAT: attribute
  constexpr reference operator*() const {
    Expects(index_ != span_->size());
    return *(span_->data() + index_);
  }

  constexpr pointer operator->() const {
    Expects(index_ != span_->size());
    return span_->data() + index_;
  }

  constexpr span_iterator &operator++() {
    Expects(0 <= index_ && index_ != span_->size());
    ++index_;
    return *this;
  }

  constexpr span_iterator operator++(int) {
    auto ret = *this;
    ++(*this);
    return ret;
  }

  constexpr span_iterator &operator--() {
    Expects(index_ != 0 && index_ <= span_->size());
    --index_;
    return *this;
  }

  constexpr span_iterator operator--(int) {
    auto ret = *this;
    --(*this);
    return ret;
  }

  constexpr span_iterator operator+(difference_type n) const {
    auto ret = *this;
    return ret += n;
  }

  friend constexpr span_iterator operator+(difference_type n,
                                           span_iterator const &rhs) {
    return rhs + n;
  }

  constexpr span_iterator &operator+=(difference_type n) {
    Expects((index_ + n) >= 0 && (index_ + n) <= span_->size());
    index_ += n;
    return *this;
  }

  constexpr span_iterator operator-(difference_type n) const {
    auto ret = *this;
    return ret -= n;
  }

  constexpr span_iterator &operator-=(difference_type n) { return *this += -n; }

  constexpr difference_type operator-(span_iterator rhs) const {
    Expects(span_ == rhs.span_);
    return index_ - rhs.index_;
  }

  constexpr reference operator[](difference_type n) const {
    return *(*this + n);
  }

  constexpr friend bool operator==(span_iterator lhs,
                                   span_iterator rhs) noexcept {
    return lhs.span_ == rhs.span_ && lhs.index_ == rhs.index_;
  }

  constexpr friend bool operator!=(span_iterator lhs,
                                   span_iterator rhs) noexcept {
    return !(lhs == rhs);
  }

  constexpr friend bool operator<(span_iterator lhs,
                                  span_iterator rhs) noexcept {
    return lhs.index_ < rhs.index_;
  }

  constexpr friend bool operator<=(span_iterator lhs,
                                   span_iterator rhs) noexcept {
    return !(rhs < lhs);
  }

  constexpr friend bool operator>(span_iterator lhs,
                                  span_iterator rhs) noexcept {
    return rhs < lhs;
  }

  constexpr friend bool operator>=(span_iterator lhs,
                                   span_iterator rhs) noexcept {
    return !(rhs > lhs);
  }

#ifdef _MSC_VER
  // MSVC++ iterator debugging support; allows STL algorithms in 15.8+
  // to unwrap span_iterator to a pointer type after a range check in STL
  // algorithm calls
  friend constexpr void
  _Verify_range(span_iterator lhs,
                span_iterator rhs) noexcept { // test that [lhs, rhs) forms a
                                              // valid range inside an STL
                                              // algorithm
    Expects(lhs.span_ == rhs.span_            // range spans have to match
            && lhs.index_ <= rhs.index_);     // range must not be transposed
  }

  constexpr void _Verify_offset(const difference_type n) const
      noexcept { // test that the iterator *this + n is a valid range in an STL
    // algorithm call
    Expects((index_ + n) >= 0 && (index_ + n) <= span_->size());
  }

  GSL_SUPPRESS(bounds .1) // NO-FORMAT: attribute
  constexpr pointer _Unwrapped() const
      noexcept { // after seeking *this to a high water mark, or using one of
                 // the
    // _Verify_xxx functions above, unwrap this span_iterator to a raw
    // pointer
    return span_->data() + index_;
  }

    // Tell the STL that span_iterator should not be unwrapped if it can't
    // validate in advance, even in release / optimized builds:
#if defined(GSL_USE_STATIC_CONSTEXPR_WORKAROUND)
  static constexpr const bool _Unwrap_when_unverified = false;
#else
  static constexpr bool _Unwrap_when_unverified = false;
#endif
  GSL_SUPPRESS(con .3) // NO-FORMAT: attribute // TODO: false positive
  constexpr void _Seek_to(const pointer p) noexcept { // adjust the position of
                                                      // *this to previously
                                                      // verified location p
    // after _Unwrapped
    index_ = p - span_->data();
  }
#endif

protected:
  const Span *span_ = nullptr;
  std::ptrdiff_t index_ = 0;
};

template <std::ptrdiff_t Ext> class extent_type {
public:
  using index_type = std::ptrdiff_t;

  static_assert(Ext >= 0, "A fixed-size span must be >= 0 in size.");

  constexpr extent_type() noexcept {}

  template <index_type Other> constexpr extent_type(extent_type<Other> ext) {
    static_assert(
        Other == Ext || Other == dynamic_extent,
        "Mismatch between fixed-size extent and size of initializing data.");
    Expects(ext.size() == Ext);
  }

  constexpr extent_type(index_type size) { Expects(size == Ext); }

  constexpr index_type size() const noexcept { return Ext; }
};

template <> class extent_type<dynamic_extent> {
public:
  using index_type = std::ptrdiff_t;

  template <index_type Other>
  explicit constexpr extent_type(extent_type<Other> ext) : size_(ext.size()) {}

  explicit constexpr extent_type(index_type size) : size_(size) {
    Expects(size >= 0);
  }

  constexpr index_type size() const noexcept { return size_; }

private:
  index_type size_;
};

template <class ElementType, std::ptrdiff_t Extent, std::ptrdiff_t Offset,
          std::ptrdiff_t Count>
struct calculate_subspan_type {
  using type =
      span<ElementType,
           Count != dynamic_extent
               ? Count
               : (Extent != dynamic_extent ? Extent - Offset : Extent)>;
};
} // namespace details

// [span], class template span
template <class ElementType, std::ptrdiff_t Extent> class span {
public:
  // constants and types
  using element_type = ElementType;
  using value_type = std::remove_cv_t<ElementType>;
  using index_type = std::ptrdiff_t;
  using pointer = element_type *;
  using reference = element_type &;

  using iterator = details::span_iterator<span<ElementType, Extent>, false>;
  using const_iterator =
      details::span_iterator<span<ElementType, Extent>, true>;
  using reverse_iterator = std::reverse_iterator<iterator>;
  using const_reverse_iterator = std::reverse_iterator<const_iterator>;

  using size_type = index_type;

#if defined(GSL_USE_STATIC_CONSTEXPR_WORKAROUND)
  static constexpr const index_type extent{Extent};
#else
  static constexpr index_type extent{Extent};
#endif

  // [span.cons], span constructors, copy, assignment, and destructor
  template <bool Dependent = false,
            // "Dependent" is needed to make "std::enable_if_t<Dependent ||
            // Extent <= 0>" SFINAE, since "std::enable_if_t<Extent <= 0>" is
            // ill-formed when Extent is greater than 0.
            class = std::enable_if_t<(Dependent || Extent <= 0)>>
  constexpr span() noexcept : storage_(nullptr, details::extent_type<0>()) {}

  constexpr span(pointer ptr, index_type count) : storage_(ptr, count) {}

  constexpr span(pointer firstElem, pointer lastElem)
      : storage_(firstElem, std::distance(firstElem, lastElem)) {}

  template <std::size_t N>
  constexpr span(element_type (&arr)[N]) noexcept
      : storage_(KnownNotNull{std::addressof(arr[0])},
                 details::extent_type<N>()) {}

  template <std::size_t N, class = std::enable_if_t<(N > 0)>>
  constexpr span(std::array<std::remove_const_t<element_type>, N> &arr) noexcept
      : storage_(KnownNotNull{arr.data()}, details::extent_type<N>()) {}

  constexpr span(std::array<std::remove_const_t<element_type>, 0> &) noexcept
      : storage_(static_cast<pointer>(nullptr), details::extent_type<0>()) {}

  template <std::size_t N, class = std::enable_if_t<(N > 0)>>
  constexpr span(
      const std::array<std::remove_const_t<element_type>, N> &arr) noexcept
      : storage_(KnownNotNull{arr.data()}, details::extent_type<N>()) {}

  constexpr span(
      const std::array<std::remove_const_t<element_type>, 0> &) noexcept
      : storage_(static_cast<pointer>(nullptr), details::extent_type<0>()) {}

  // NB: the SFINAE here uses .data() as a incomplete/imperfect proxy for the
  // requirement on Container to be a contiguous sequence container.
  template <
      class Container,
      class = std::enable_if_t<
          !details::is_span<Container>::value &&
          !details::is_std_array<Container>::value &&
          std::is_convertible<typename Container::pointer, pointer>::value &&
          std::is_convertible<
              typename Container::pointer,
              decltype(std::declval<Container>().data())>::value>>
  constexpr span(Container &cont)
      : span(cont.data(), narrow<index_type>(cont.size())) {}

  template <
      class Container,
      class = std::enable_if_t<
          std::is_const<element_type>::value &&
          !details::is_span<Container>::value &&
          std::is_convertible<typename Container::pointer, pointer>::value &&
          std::is_convertible<
              typename Container::pointer,
              decltype(std::declval<Container>().data())>::value>>
  constexpr span(const Container &cont)
      : span(cont.data(), narrow<index_type>(cont.size())) {}

  constexpr span(const span &other) noexcept = default;

  template <
      class OtherElementType, std::ptrdiff_t OtherExtent,
      class = std::enable_if_t<
          details::is_allowed_extent_conversion<OtherExtent, Extent>::value &&
          details::is_allowed_element_type_conversion<OtherElementType,
                                                      element_type>::value>>
  constexpr span(const span<OtherElementType, OtherExtent> &other)
      : storage_(other.data(),
                 details::extent_type<OtherExtent>(other.size())) {}

  ~span() noexcept = default;
  constexpr span &operator=(const span &other) noexcept = default;

  // [span.sub], span subviews
  template <std::ptrdiff_t Count>
  constexpr span<element_type, Count> first() const {
    Expects(Count >= 0 && Count <= size());
    return {data(), Count};
  }

  template <std::ptrdiff_t Count>
  GSL_SUPPRESS(bounds .1) // NO-FORMAT: attribute
  constexpr span<element_type, Count> last() const {
    Expects(Count >= 0 && size() - Count >= 0);
    return {data() + (size() - Count), Count};
  }

  template <std::ptrdiff_t Offset, std::ptrdiff_t Count = dynamic_extent>
  GSL_SUPPRESS(bounds .1) // NO-FORMAT: attribute
  constexpr auto subspan() const ->
      typename details::calculate_subspan_type<ElementType, Extent, Offset,
                                               Count>::type {
    Expects(
        (Offset >= 0 && size() - Offset >= 0) &&
        (Count == dynamic_extent || (Count >= 0 && Offset + Count <= size())));

    return {data() + Offset, Count == dynamic_extent ? size() - Offset : Count};
  }

  constexpr span<element_type, dynamic_extent> first(index_type count) const {
    Expects(count >= 0 && count <= size());
    return {data(), count};
  }

  constexpr span<element_type, dynamic_extent> last(index_type count) const {
    return make_subspan(size() - count, dynamic_extent,
                        subspan_selector<Extent>{});
  }

  constexpr span<element_type, dynamic_extent>
  subspan(index_type offset, index_type count = dynamic_extent) const {
    return make_subspan(offset, count, subspan_selector<Extent>{});
  }

  // [span.obs], span observers
  constexpr index_type size() const noexcept { return storage_.size(); }
  constexpr index_type size_bytes() const noexcept {
    return size() * narrow_cast<index_type>(sizeof(element_type));
  }
  constexpr bool empty() const noexcept { return size() == 0; }

  // [span.elem], span element access
  GSL_SUPPRESS(bounds .1) // NO-FORMAT: attribute
  constexpr reference operator[](index_type idx) const {
    Expects(CheckRange(idx, storage_.size()));
    return data()[idx];
  }

  constexpr reference at(index_type idx) const { return this->operator[](idx); }
  constexpr reference operator()(index_type idx) const {
    return this->operator[](idx);
  }
  constexpr pointer data() const noexcept { return storage_.data(); }

  // [span.iter], span iterator support
  constexpr iterator begin() const noexcept { return {this, 0}; }
  constexpr iterator end() const noexcept { return {this, size()}; }

  constexpr const_iterator cbegin() const noexcept { return {this, 0}; }
  constexpr const_iterator cend() const noexcept { return {this, size()}; }

  constexpr reverse_iterator rbegin() const noexcept {
    return reverse_iterator{end()};
  }
  constexpr reverse_iterator rend() const noexcept {
    return reverse_iterator{begin()};
  }

  constexpr const_reverse_iterator crbegin() const noexcept {
    return const_reverse_iterator{cend()};
  }
  constexpr const_reverse_iterator crend() const noexcept {
    return const_reverse_iterator{cbegin()};
  }

#ifdef _MSC_VER
  // Tell MSVC how to unwrap spans in range-based-for
  constexpr pointer _Unchecked_begin() const noexcept { return data(); }
  constexpr pointer _Unchecked_end() const noexcept {
    GSL_SUPPRESS(bounds .1) // NO-FORMAT: attribute
    return data() + size();
  }
#endif // _MSC_VER

private:
  static constexpr bool CheckRange(index_type idx, index_type size) noexcept {
  // Optimization:
  //
  // idx >= 0 && idx < size
  // =>
  // static_cast<size_t>(idx) < static_cast<size_t>(size)
  //
  // because size >=0 by span construction, and negative idx will
  // wrap around to a value always greater than size when casted.

  // check if we have enough space to wrap around
#if defined(__cpp_if_constexpr)
    if constexpr (sizeof(index_type) <= sizeof(size_t))
#else
    if (sizeof(index_type) <= sizeof(size_t))
#endif
    {
      return narrow_cast<size_t>(idx) < narrow_cast<size_t>(size);
    } else {
      return idx >= 0 && idx < size;
    }
  }

  // Needed to remove unnecessary null check in subspans
  struct KnownNotNull {
    pointer p;
  };

  // this implementation detail class lets us take advantage of the
  // empty base class optimization to pay for only storage of a single
  // pointer in the case of fixed-size spans
  template <class ExtentType> class storage_type : public ExtentType {
  public:
    // KnownNotNull parameter is needed to remove unnecessary null check
    // in subspans and constructors from arrays
    template <class OtherExtentType>
    constexpr storage_type(KnownNotNull data, OtherExtentType ext)
        : ExtentType(ext), data_(data.p) {
      Expects(ExtentType::size() >= 0);
    }

    template <class OtherExtentType>
    constexpr storage_type(pointer data, OtherExtentType ext)
        : ExtentType(ext), data_(data) {
      Expects(ExtentType::size() >= 0);
      Expects(data || ExtentType::size() == 0);
    }

    constexpr pointer data() const noexcept { return data_; }

  private:
    pointer data_;
  };

  storage_type<details::extent_type<Extent>> storage_;

  // The rest is needed to remove unnecessary null check
  // in subspans and constructors from arrays
  constexpr span(KnownNotNull ptr, index_type count) : storage_(ptr, count) {}

  template <std::ptrdiff_t CallerExtent> class subspan_selector {};

  template <std::ptrdiff_t CallerExtent>
  span<element_type, dynamic_extent>
  make_subspan(index_type offset, index_type count,
               subspan_selector<CallerExtent>) const {
    const span<element_type, dynamic_extent> tmp(*this);
    return tmp.subspan(offset, count);
  }

  GSL_SUPPRESS(bounds .1) // NO-FORMAT: attribute
  span<element_type, dynamic_extent>
  make_subspan(index_type offset, index_type count,
               subspan_selector<dynamic_extent>) const {
    Expects(offset >= 0 && size() - offset >= 0);

    if (count == dynamic_extent) {
      return {KnownNotNull{data() + offset}, size() - offset};
    }

    Expects(count >= 0 && size() - offset >= count);
    return {KnownNotNull{data() + offset}, count};
  }
};

#if defined(GSL_USE_STATIC_CONSTEXPR_WORKAROUND)
template <class ElementType, std::ptrdiff_t Extent>
constexpr const typename span<ElementType, Extent>::index_type
    span<ElementType, Extent>::extent;
#endif

// [span.comparison], span comparison operators
template <class ElementType, std::ptrdiff_t FirstExtent,
          std::ptrdiff_t SecondExtent>
constexpr bool operator==(span<ElementType, FirstExtent> l,
                          span<ElementType, SecondExtent> r) {
  return std::equal(l.begin(), l.end(), r.begin(), r.end());
}

template <class ElementType, std::ptrdiff_t Extent>
constexpr bool operator!=(span<ElementType, Extent> l,
                          span<ElementType, Extent> r) {
  return !(l == r);
}

template <class ElementType, std::ptrdiff_t Extent>
constexpr bool operator<(span<ElementType, Extent> l,
                         span<ElementType, Extent> r) {
  return std::lexicographical_compare(l.begin(), l.end(), r.begin(), r.end());
}

template <class ElementType, std::ptrdiff_t Extent>
constexpr bool operator<=(span<ElementType, Extent> l,
                          span<ElementType, Extent> r) {
  return !(l > r);
}

template <class ElementType, std::ptrdiff_t Extent>
constexpr bool operator>(span<ElementType, Extent> l,
                         span<ElementType, Extent> r) {
  return r < l;
}

template <class ElementType, std::ptrdiff_t Extent>
constexpr bool operator>=(span<ElementType, Extent> l,
                          span<ElementType, Extent> r) {
  return !(l < r);
}

namespace details {
// if we only supported compilers with good constexpr support then
// this pair of classes could collapse down to a constexpr function

// we should use a narrow_cast<> to go to std::size_t, but older compilers may
// not see it as constexpr and so will fail compilation of the template
template <class ElementType, std::ptrdiff_t Extent>
struct calculate_byte_size
    : std::integral_constant<std::ptrdiff_t,
                             static_cast<std::ptrdiff_t>(
                                 sizeof(ElementType) *
                                 static_cast<std::size_t>(Extent))> {};

template <class ElementType>
struct calculate_byte_size<ElementType, dynamic_extent>
    : std::integral_constant<std::ptrdiff_t, dynamic_extent> {};
} // namespace details

// [span.objectrep], views of object representation
template <class ElementType, std::ptrdiff_t Extent>
span<const byte, details::calculate_byte_size<ElementType, Extent>::value>
as_bytes(span<ElementType, Extent> s) noexcept {
  GSL_SUPPRESS(type .1) // NO-FORMAT: attribute
  return {reinterpret_cast<const byte *>(s.data()), s.size_bytes()};
}

template <class ElementType, std::ptrdiff_t Extent,
          class = std::enable_if_t<!std::is_const<ElementType>::value>>
span<byte, details::calculate_byte_size<ElementType, Extent>::value>
as_writeable_bytes(span<ElementType, Extent> s) noexcept {
  GSL_SUPPRESS(type .1) // NO-FORMAT: attribute
  return {reinterpret_cast<byte *>(s.data()), s.size_bytes()};
}

//
// make_span() - Utility functions for creating spans
//
template <class ElementType>
constexpr span<ElementType>
make_span(ElementType *ptr, typename span<ElementType>::index_type count) {
  return span<ElementType>(ptr, count);
}

template <class ElementType>
constexpr span<ElementType> make_span(ElementType *firstElem,
                                      ElementType *lastElem) {
  return span<ElementType>(firstElem, lastElem);
}

template <class ElementType, std::size_t N>
constexpr span<ElementType, N> make_span(ElementType (&arr)[N]) noexcept {
  return span<ElementType, N>(arr);
}

template <class Container>
constexpr span<typename Container::value_type> make_span(Container &cont) {
  return span<typename Container::value_type>(cont);
}

template <class Container>
constexpr span<const typename Container::value_type>
make_span(const Container &cont) {
  return span<const typename Container::value_type>(cont);
}

template <class Ptr>
constexpr span<typename Ptr::element_type> make_span(Ptr &cont,
                                                     std::ptrdiff_t count) {
  return span<typename Ptr::element_type>(cont, count);
}

template <class Ptr>
constexpr span<typename Ptr::element_type> make_span(Ptr &cont) {
  return span<typename Ptr::element_type>(cont);
}

// Specialization of gsl::at for span
template <class ElementType, std::ptrdiff_t Extent>
constexpr ElementType &at(span<ElementType, Extent> s, index i) {
  // No bounds checking here because it is done in span::operator[] called below
  return s[i];
}

} // namespace gsl

#if defined(_MSC_VER) && !defined(__clang__)
#if _MSC_VER < 1910
#undef constexpr
#pragma pop_macro("constexpr")

#endif // _MSC_VER < 1910

#pragma warning(pop)
#endif // _MSC_VER

#if defined(__GNUC__) && __GNUC__ > 6
#pragma GCC diagnostic pop
#endif // __GNUC__ > 6

#endif // GSL_SPAN_H
//------------------------------------------------------------------------------
// END span
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
// Start multi_span
//------------------------------------------------------------------------------
///////////////////////////////////////////////////////////////////////////////
//
// Copyright (c) 2015 Microsoft Corporation. All rights reserved.
//
// This code is licensed under the MIT License (MIT).
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
// THE SOFTWARE.
//
///////////////////////////////////////////////////////////////////////////////

#ifndef GSL_MULTI_SPAN_H
#define GSL_MULTI_SPAN_H

#include <algorithm> // for transform, lexicographical_compare
#include <array>     // for array
#include <cassert>
#include <cstddef>          // for ptrdiff_t, size_t, nullptr_t
#include <cstdint>          // for PTRDIFF_MAX
#include <functional>       // for divides, multiplies, minus, negate, plus
#include <initializer_list> // for initializer_list
#include <iterator>         // for iterator, random_access_iterator_tag
#include <limits>           // for numeric_limits
#include <new>
#include <numeric>
#include <stdexcept>
#include <string>      // for basic_string
#include <type_traits> // for enable_if_t, remove_cv_t, is_same, is_co...
#include <utility>

#if defined(_MSC_VER) && !defined(__clang__)

// turn off some warnings that are noisy about our Expects statements
#pragma warning(push)
#pragma warning(disable : 4127) // conditional expression is constant
#pragma warning(disable : 4702) // unreachable code

// Turn MSVC /analyze rules that generate too much noise. TODO: fix in the tool.
#pragma warning(                                                               \
    disable : 26495) // uninitalized member when constructor calls constructor
#pragma warning(                                                               \
    disable : 26473) // in some instantiations we cast to the same type
#pragma warning(                                                               \
    disable : 26490) // TODO: bug in parser - attributes and templates
#pragma warning(disable : 26465) // TODO: bug - suppression does not work on
                                 // template functions

#if _MSC_VER < 1910
#pragma push_macro("constexpr")
#define constexpr /*constexpr*/

#endif // _MSC_VER < 1910
#endif // _MSC_VER

// GCC 7 does not like the signed unsigned missmatch (size_t ptrdiff_t)
// While there is a conversion from signed to unsigned, it happens at
// compiletime, so the compiler wouldn't have to warn indiscriminently, but
// could check if the source value actually doesn't fit into the target type
// and only warn in those cases.
#if defined(__GNUC__) && __GNUC__ > 6
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wsign-conversion"
#endif

namespace gsl {

/*
** begin definitions of index and bounds
*/
namespace details {
template <typename SizeType> struct SizeTypeTraits {
  static const SizeType max_value = std::numeric_limits<SizeType>::max();
};

template <typename... Ts>
class are_integral : public std::integral_constant<bool, true> {};

template <typename T, typename... Ts>
class are_integral<T, Ts...>
    : public std::integral_constant<bool, std::is_integral<T>::value &&
                                              are_integral<Ts...>::value> {};
} // namespace details

template <std::size_t Rank> class multi_span_index final {
  static_assert(Rank > 0, "Rank must be greater than 0!");

  template <std::size_t OtherRank> friend class multi_span_index;

public:
  static const std::size_t rank = Rank;
  using value_type = std::ptrdiff_t;
  using size_type = value_type;
  using reference = std::add_lvalue_reference_t<value_type>;
  using const_reference =
      std::add_lvalue_reference_t<std::add_const_t<value_type>>;

  constexpr multi_span_index() noexcept {}

  constexpr multi_span_index(const value_type (&values)[Rank]) noexcept {
    GSL_SUPPRESS(bounds .1) // NO-FORMAT: attribute
    GSL_SUPPRESS(bounds .3) // NO-FORMAT: attribute
    std::copy(values, values + Rank, elems);
  }

  template <typename... Ts,
            typename = std::enable_if_t<(sizeof...(Ts) == Rank) &&
                                        details::are_integral<Ts...>::value>>
  constexpr multi_span_index(Ts... ds) noexcept
      : elems{narrow_cast<value_type>(ds)...} {}

  constexpr multi_span_index(const multi_span_index &other) noexcept = default;

  constexpr multi_span_index &
  operator=(const multi_span_index &rhs) noexcept = default;

  // Preconditions: component_idx < rank
  GSL_SUPPRESS(bounds .2) // NO-FORMAT: attribute
  GSL_SUPPRESS(bounds .4) // NO-FORMAT: attribute
  constexpr reference operator[](std::size_t component_idx) {
    Expects(component_idx < Rank); // Component index must be less than rank
    return elems[component_idx];
  }

  // Preconditions: component_idx < rank
  GSL_SUPPRESS(bounds .2) // NO-FORMAT: attribute
  GSL_SUPPRESS(bounds .4) // NO-FORMAT: attribute
  constexpr const_reference operator[](std::size_t component_idx) const {
    Expects(component_idx < Rank); // Component index must be less than rank
    return elems[component_idx];
  }

  GSL_SUPPRESS(bounds .1) // NO-FORMAT: attribute
  GSL_SUPPRESS(bounds .3) // NO-FORMAT: attribute
  constexpr bool operator==(const multi_span_index &rhs) const {
    return std::equal(elems, elems + rank, rhs.elems);
  }

  constexpr bool operator!=(const multi_span_index &rhs) const {
    return !(*this == rhs);
  }

  constexpr multi_span_index operator+() const noexcept { return *this; }

  constexpr multi_span_index operator-() const {
    multi_span_index ret = *this;
    std::transform(ret, ret + rank, ret, std::negate<value_type>{});
    return ret;
  }

  constexpr multi_span_index operator+(const multi_span_index &rhs) const {
    multi_span_index ret = *this;
    ret += rhs;
    return ret;
  }

  constexpr multi_span_index operator-(const multi_span_index &rhs) const {
    multi_span_index ret = *this;
    ret -= rhs;
    return ret;
  }

  constexpr multi_span_index &operator+=(const multi_span_index &rhs) {
    GSL_SUPPRESS(bounds .1) // NO-FORMAT: attribute
    GSL_SUPPRESS(bounds .3) // NO-FORMAT: attribute
    std::transform(elems, elems + rank, rhs.elems, elems,
                   std::plus<value_type>{});
    return *this;
  }

  constexpr multi_span_index &operator-=(const multi_span_index &rhs) {
    GSL_SUPPRESS(bounds .1) // NO-FORMAT: attribute
    GSL_SUPPRESS(bounds .3) // NO-FORMAT: attribute
    std::transform(elems, elems + rank, rhs.elems, elems,
                   std::minus<value_type>{});
    return *this;
  }

  constexpr multi_span_index operator*(value_type v) const {
    multi_span_index ret = *this;
    ret *= v;
    return ret;
  }

  constexpr multi_span_index operator/(value_type v) const {
    multi_span_index ret = *this;
    ret /= v;
    return ret;
  }

  friend constexpr multi_span_index operator*(value_type v,
                                              const multi_span_index &rhs) {
    return rhs * v;
  }

  constexpr multi_span_index &operator*=(value_type v) {
    GSL_SUPPRESS(bounds .1) // NO-FORMAT: attribute
    GSL_SUPPRESS(bounds .3) // NO-FORMAT: attribute
    std::transform(elems, elems + rank, elems, [v](value_type x) {
      return std::multiplies<value_type>{}(x, v);
    });
    return *this;
  }

  constexpr multi_span_index &operator/=(value_type v) {
    GSL_SUPPRESS(bounds .1) // NO-FORMAT: attribute
    GSL_SUPPRESS(bounds .3) // NO-FORMAT: attribute
    std::transform(elems, elems + rank, elems, [v](value_type x) {
      return std::divides<value_type>{}(x, v);
    });
    return *this;
  }

private:
  value_type elems[Rank] = {};
};

#if !defined(_MSC_VER) || _MSC_VER >= 1910

struct static_bounds_dynamic_range_t {
  template <typename T, typename = std::enable_if_t<std::is_integral<T>::value>>
  constexpr operator T() const noexcept {
    return narrow_cast<T>(-1);
  }
};

constexpr bool operator==(static_bounds_dynamic_range_t,
                          static_bounds_dynamic_range_t) noexcept {
  return true;
}

constexpr bool operator!=(static_bounds_dynamic_range_t,
                          static_bounds_dynamic_range_t) noexcept {
  return false;
}

template <typename T, typename = std::enable_if_t<std::is_integral<T>::value>>
constexpr bool operator==(static_bounds_dynamic_range_t, T other) noexcept {
  return narrow_cast<T>(-1) == other;
}

template <typename T, typename = std::enable_if_t<std::is_integral<T>::value>>
constexpr bool operator==(T left,
                          static_bounds_dynamic_range_t right) noexcept {
  return right == left;
}

template <typename T, typename = std::enable_if_t<std::is_integral<T>::value>>
constexpr bool operator!=(static_bounds_dynamic_range_t, T other) noexcept {
  return narrow_cast<T>(-1) != other;
}

template <typename T, typename = std::enable_if_t<std::is_integral<T>::value>>
constexpr bool operator!=(T left,
                          static_bounds_dynamic_range_t right) noexcept {
  return right != left;
}

constexpr static_bounds_dynamic_range_t dynamic_range{};
#else
const std::ptrdiff_t dynamic_range = -1;
#endif

struct generalized_mapping_tag {};
struct contiguous_mapping_tag : generalized_mapping_tag {};

namespace details {

template <std::ptrdiff_t Left, std::ptrdiff_t Right> struct LessThan {
  static const bool value = Left < Right;
};

template <std::ptrdiff_t... Ranges> struct BoundsRanges {
  using size_type = std::ptrdiff_t;
  static const size_type Depth = 0;
  static const size_type DynamicNum = 0;
  static const size_type CurrentRange = 1;
  static const size_type TotalSize = 1;

  // TODO : following signature is for work around VS bug
  template <typename OtherRange>
  constexpr BoundsRanges(const OtherRange &, bool /* firstLevel */) {}

  constexpr BoundsRanges(const std::ptrdiff_t *const) {}
  constexpr BoundsRanges() noexcept = default;

  template <typename T, std::size_t Dim> constexpr void serialize(T &) const {}

  template <typename T, std::size_t Dim>
  constexpr size_type linearize(const T &) const {
    return 0;
  }

  template <typename T, std::size_t Dim>
  constexpr size_type contains(const T &) const {
    return -1;
  }

  constexpr size_type elementNum(std::size_t) const noexcept { return 0; }

  constexpr size_type totalSize() const noexcept { return TotalSize; }

  constexpr bool operator==(const BoundsRanges &) const noexcept {
    return true;
  }
};

template <std::ptrdiff_t... RestRanges>
struct BoundsRanges<dynamic_range, RestRanges...>
    : BoundsRanges<RestRanges...> {
  using Base = BoundsRanges<RestRanges...>;
  using size_type = std::ptrdiff_t;
  static const std::size_t Depth = Base::Depth + 1;
  static const std::size_t DynamicNum = Base::DynamicNum + 1;
  static const size_type CurrentRange = dynamic_range;
  static const size_type TotalSize = dynamic_range;

private:
  size_type m_bound;

public:
  GSL_SUPPRESS(f .23) // NO-FORMAT: attribute // this pointer type is cannot be
                      // assigned nullptr - issue in not_null
  GSL_SUPPRESS(bounds .1) // NO-FORMAT: attribute
  constexpr BoundsRanges(const std::ptrdiff_t *const arr)
      : Base(arr + 1), m_bound(*arr * this->Base::totalSize()) {
    Expects(0 <= *arr);
  }

  constexpr BoundsRanges() noexcept : m_bound(0) {}

  template <std::ptrdiff_t OtherRange, std::ptrdiff_t... RestOtherRanges>
  constexpr BoundsRanges(
      const BoundsRanges<OtherRange, RestOtherRanges...> &other,
      bool /* firstLevel */ = true)
      : Base(static_cast<const BoundsRanges<RestOtherRanges...> &>(other),
             false),
        m_bound(other.totalSize()) {}

  template <typename T, std::size_t Dim = 0>
  constexpr void serialize(T &arr) const {
    arr[Dim] = elementNum();
    this->Base::template serialize<T, Dim + 1>(arr);
  }

  template <typename T, std::size_t Dim = 0>
  GSL_SUPPRESS(bounds .4) // NO-FORMAT: attribute
  constexpr size_type linearize(const T &arr) const {
    const size_type index = this->Base::totalSize() * arr[Dim];
    Expects(index < m_bound);
    return index + this->Base::template linearize<T, Dim + 1>(arr);
  }

  template <typename T, std::size_t Dim = 0>
  constexpr size_type contains(const T &arr) const {
    const ptrdiff_t last = this->Base::template contains<T, Dim + 1>(arr);
    if (last == -1)
      return -1;
    const ptrdiff_t cur = this->Base::totalSize() * arr[Dim];
    return cur < m_bound ? cur + last : -1;
  }

  GSL_SUPPRESS(c .128) // NO-FORMAT: attribute // no pointers to BoundsRanges
                       // should be ever used
  constexpr size_type totalSize() const noexcept { return m_bound; }

  GSL_SUPPRESS(c .128) // NO-FORMAT: attribute // no pointers to BoundsRanges
                       // should be ever used
  constexpr size_type elementNum() const noexcept {
    return totalSize() / this->Base::totalSize();
  }

  GSL_SUPPRESS(c .128) // NO-FORMAT: attribute // no pointers to BoundsRanges
                       // should be ever used
  constexpr size_type elementNum(std::size_t dim) const noexcept {
    if (dim > 0)
      return this->Base::elementNum(dim - 1);
    else
      return elementNum();
  }

  constexpr bool operator==(const BoundsRanges &rhs) const noexcept {
    return m_bound == rhs.m_bound &&
           static_cast<const Base &>(*this) == static_cast<const Base &>(rhs);
  }
};

template <std::ptrdiff_t CurRange, std::ptrdiff_t... RestRanges>
struct BoundsRanges<CurRange, RestRanges...> : BoundsRanges<RestRanges...> {
  using Base = BoundsRanges<RestRanges...>;
  using size_type = std::ptrdiff_t;
  static const std::size_t Depth = Base::Depth + 1;
  static const std::size_t DynamicNum = Base::DynamicNum;
  static const size_type CurrentRange = CurRange;
  static const size_type TotalSize = Base::TotalSize == dynamic_range
                                         ? dynamic_range
                                         : CurrentRange * Base::TotalSize;

  constexpr BoundsRanges(const std::ptrdiff_t *const arr) : Base(arr) {}
  constexpr BoundsRanges() = default;

  template <std::ptrdiff_t OtherRange, std::ptrdiff_t... RestOtherRanges>
  constexpr BoundsRanges(
      const BoundsRanges<OtherRange, RestOtherRanges...> &other,
      bool firstLevel = true)
      : Base(static_cast<const BoundsRanges<RestOtherRanges...> &>(other),
             false) {
    GSL_SUPPRESS(type .4) // NO-FORMAT: attribute // TODO: false positive
    (void)firstLevel;
  }

  template <typename T, std::size_t Dim = 0>
  constexpr void serialize(T &arr) const {
    arr[Dim] = elementNum();
    this->Base::template serialize<T, Dim + 1>(arr);
  }

  template <typename T, std::size_t Dim = 0>
  constexpr size_type linearize(const T &arr) const {
    GSL_SUPPRESS(bounds .4)                            // NO-FORMAT: attribute
    Expects(arr[Dim] >= 0 && arr[Dim] < CurrentRange); // Index is out of range
    GSL_SUPPRESS(bounds .4)                            // NO-FORMAT: attribute
    const ptrdiff_t d = arr[Dim];
    return this->Base::totalSize() * d +
           this->Base::template linearize<T, Dim + 1>(arr);
  }

  template <typename T, std::size_t Dim = 0>
  constexpr size_type contains(const T &arr) const {
    if (arr[Dim] >= CurrentRange)
      return -1;
    const size_type last = this->Base::template contains<T, Dim + 1>(arr);
    if (last == -1)
      return -1;
    return this->Base::totalSize() * arr[Dim] + last;
  }

  GSL_SUPPRESS(c .128) // NO-FORMAT: attribute // no pointers to BoundsRanges
                       // should be ever used
  constexpr size_type totalSize() const noexcept {
    return CurrentRange * this->Base::totalSize();
  }

  GSL_SUPPRESS(c .128) // NO-FORMAT: attribute // no pointers to BoundsRanges
                       // should be ever used
  constexpr size_type elementNum() const noexcept { return CurrentRange; }

  GSL_SUPPRESS(c .128) // NO-FORMAT: attribute // no pointers to BoundsRanges
                       // should be ever used
  constexpr size_type elementNum(std::size_t dim) const noexcept {
    if (dim > 0)
      return this->Base::elementNum(dim - 1);
    else
      return elementNum();
  }

  constexpr bool operator==(const BoundsRanges &rhs) const noexcept {
    return static_cast<const Base &>(*this) == static_cast<const Base &>(rhs);
  }
};

template <typename SourceType, typename TargetType>
struct BoundsRangeConvertible
    : public std::integral_constant<
          bool, (SourceType::TotalSize >= TargetType::TotalSize ||
                 TargetType::TotalSize == dynamic_range ||
                 SourceType::TotalSize == dynamic_range ||
                 TargetType::TotalSize == 0)> {};

template <typename TypeChain> struct TypeListIndexer {
  const TypeChain &obj_;
  TypeListIndexer(const TypeChain &obj) : obj_(obj) {}

  template <std::size_t N> const TypeChain &getObj(std::true_type) {
    return obj_;
  }

  template <std::size_t N, typename MyChain = TypeChain,
            typename MyBase = typename MyChain::Base>
  auto getObj(std::false_type)
      -> decltype(TypeListIndexer<MyBase>(static_cast<const MyBase &>(obj_))
                      .template get<N>()) {
    return TypeListIndexer<MyBase>(static_cast<const MyBase &>(obj_))
        .template get<N>();
  }

  template <std::size_t N>
  auto get()
      -> decltype(getObj<N - 1>(std::integral_constant<bool, N == 0>())) {
    return getObj<N - 1>(std::integral_constant<bool, N == 0>());
  }
};

template <typename TypeChain>
TypeListIndexer<TypeChain> createTypeListIndexer(const TypeChain &obj) {
  return TypeListIndexer<TypeChain>(obj);
}

template <std::size_t Rank, bool Enabled = (Rank > 1),
          typename Ret = std::enable_if_t<Enabled, multi_span_index<Rank - 1>>>
constexpr Ret shift_left(const multi_span_index<Rank> &other) noexcept {
  Ret ret{};
  for (std::size_t i = 0; i < Rank - 1; ++i) {
    GSL_SUPPRESS(bounds .4) // NO-FORMAT: attribute
    ret[i] = other[i + 1];
  }
  return ret;
}
} // namespace details

template <typename IndexType> class bounds_iterator;

template <std::ptrdiff_t... Ranges> class static_bounds {
public:
  static_bounds(const details::BoundsRanges<Ranges...> &) {}
};

template <std::ptrdiff_t FirstRange, std::ptrdiff_t... RestRanges>
class static_bounds<FirstRange, RestRanges...> {
  using MyRanges = details::BoundsRanges<FirstRange, RestRanges...>;

  MyRanges m_ranges;
  constexpr static_bounds(const MyRanges &range) noexcept : m_ranges(range) {}

  template <std::ptrdiff_t... OtherRanges> friend class static_bounds;

public:
  static const std::size_t rank = MyRanges::Depth;
  static const std::size_t dynamic_rank = MyRanges::DynamicNum;
  static const std::ptrdiff_t static_size = MyRanges::TotalSize;

  using size_type = std::ptrdiff_t;
  using index_type = multi_span_index<rank>;
  using const_index_type = std::add_const_t<index_type>;
  using iterator = bounds_iterator<const_index_type>;
  using const_iterator = bounds_iterator<const_index_type>;
  using difference_type = std::ptrdiff_t;
  using sliced_type = static_bounds<RestRanges...>;
  using mapping_type = contiguous_mapping_tag;

  constexpr static_bounds() /*noexcept*/ = default;

  template <typename SourceType, typename TargetType, std::size_t Rank>
  struct BoundsRangeConvertible2;

  template <std::size_t Rank, typename SourceType, typename TargetType,
            typename Ret = BoundsRangeConvertible2<
                typename SourceType::Base, typename TargetType::Base, Rank>>
  static auto helpBoundsRangeConvertible(SourceType, TargetType, std::true_type)
      -> Ret;

  template <std::size_t Rank, typename SourceType, typename TargetType>
  static auto helpBoundsRangeConvertible(SourceType, TargetType, ...)
      -> std::false_type;

  template <typename SourceType, typename TargetType, std::size_t Rank>
  struct BoundsRangeConvertible2
      : decltype(helpBoundsRangeConvertible<Rank - 1>(
            SourceType(), TargetType(),
            std::integral_constant<
                bool,
                SourceType::Depth == TargetType::Depth &&
                    (SourceType::CurrentRange == TargetType::CurrentRange ||
                     TargetType::CurrentRange == dynamic_range ||
                     SourceType::CurrentRange == dynamic_range)>())) {};

  template <typename SourceType, typename TargetType>
  struct BoundsRangeConvertible2<SourceType, TargetType, 0> : std::true_type {};

  template <typename SourceType, typename TargetType,
            std::ptrdiff_t Rank = TargetType::Depth>
  struct BoundsRangeConvertible
      : decltype(helpBoundsRangeConvertible<Rank - 1>(
            SourceType(), TargetType(),
            std::integral_constant<
                bool,
                SourceType::Depth == TargetType::Depth &&
                    (!details::LessThan<SourceType::CurrentRange,
                                        TargetType::CurrentRange>::value ||
                     TargetType::CurrentRange == dynamic_range ||
                     SourceType::CurrentRange == dynamic_range)>())) {};

  template <typename SourceType, typename TargetType>
  struct BoundsRangeConvertible<SourceType, TargetType, 0> : std::true_type {};

  template <std::ptrdiff_t... Ranges,
            typename = std::enable_if_t<details::BoundsRangeConvertible<
                details::BoundsRanges<Ranges...>,
                details::BoundsRanges<FirstRange, RestRanges...>>::value>>
  constexpr static_bounds(const static_bounds<Ranges...> &other)
      : m_ranges(other.m_ranges) {
    Expects((MyRanges::DynamicNum == 0 &&
             details::BoundsRanges<Ranges...>::DynamicNum == 0) ||
            MyRanges::DynamicNum > 0 ||
            other.m_ranges.totalSize() >= m_ranges.totalSize());
  }

  constexpr static_bounds(std::initializer_list<size_type> il)
      : m_ranges(il.begin()) {
    // Size of the initializer list must match the rank of the array
    Expects((MyRanges::DynamicNum == 0 && il.size() == 1 &&
             *il.begin() == static_size) ||
            MyRanges::DynamicNum == il.size());
    // Size of the range must be less than the max element of the size type
    Expects(m_ranges.totalSize() <= PTRDIFF_MAX);
  }

  constexpr sliced_type slice() const noexcept {
    return sliced_type{
        static_cast<const details::BoundsRanges<RestRanges...> &>(m_ranges)};
  }

  constexpr size_type stride() const noexcept {
    return rank > 1 ? slice().size() : 1;
  }

  constexpr size_type size() const noexcept { return m_ranges.totalSize(); }

  constexpr size_type total_size() const noexcept {
    return m_ranges.totalSize();
  }

  constexpr size_type linearize(const index_type &idx) const {
    return m_ranges.linearize(idx);
  }

  constexpr bool contains(const index_type &idx) const noexcept {
    return m_ranges.contains(idx) != -1;
  }

  constexpr size_type operator[](std::size_t idx) const noexcept {
    return m_ranges.elementNum(idx);
  }

  template <std::size_t Dim = 0> constexpr size_type extent() const noexcept {
    static_assert(
        Dim < rank,
        "dimension should be less than rank (dimension count starts from 0)");
    return details::createTypeListIndexer(m_ranges)
        .template get<Dim>()
        .elementNum();
  }

  template <typename IntType> constexpr size_type extent(IntType dim) const {
    static_assert(std::is_integral<IntType>::value,
                  "Dimension parameter must be supplied as an integral type.");
    auto real_dim = narrow_cast<std::size_t>(dim);
    Expects(real_dim < rank);

    return m_ranges.elementNum(real_dim);
  }

  constexpr index_type index_bounds() const noexcept {
    size_type extents[rank] = {};
    m_ranges.serialize(extents);
    return {extents};
  }

  template <std::ptrdiff_t... Ranges>
  constexpr bool operator==(const static_bounds<Ranges...> &rhs) const
      noexcept {
    return this->size() == rhs.size();
  }

  template <std::ptrdiff_t... Ranges>
  constexpr bool operator!=(const static_bounds<Ranges...> &rhs) const
      noexcept {
    return !(*this == rhs);
  }

  constexpr const_iterator begin() const noexcept {
    return const_iterator(*this, index_type{});
  }

  constexpr const_iterator end() const noexcept {
    return const_iterator(*this, this->index_bounds());
  }
};

template <std::size_t Rank> class strided_bounds {
  template <std::size_t OtherRank> friend class strided_bounds;

public:
  static const std::size_t rank = Rank;
  using value_type = std::ptrdiff_t;
  using reference = std::add_lvalue_reference_t<value_type>;
  using const_reference = std::add_const_t<reference>;
  using size_type = value_type;
  using difference_type = value_type;
  using index_type = multi_span_index<rank>;
  using const_index_type = std::add_const_t<index_type>;
  using iterator = bounds_iterator<const_index_type>;
  using const_iterator = bounds_iterator<const_index_type>;
  static const value_type dynamic_rank = rank;
  static const value_type static_size = dynamic_range;
  using sliced_type =
      std::conditional_t<rank != 0, strided_bounds<rank - 1>, void>;
  using mapping_type = generalized_mapping_tag;

  constexpr strided_bounds(const strided_bounds &) noexcept = default;

  constexpr strided_bounds &
  operator=(const strided_bounds &) noexcept = default;

  constexpr strided_bounds(const value_type (&values)[rank], index_type strides)
      : m_extents(values), m_strides(std::move(strides)) {}

  constexpr strided_bounds(const index_type &extents,
                           const index_type &strides) noexcept
      : m_extents(extents), m_strides(strides) {}

  constexpr index_type strides() const noexcept { return m_strides; }

  GSL_SUPPRESS(bounds .4) // NO-FORMAT: attribute
  constexpr size_type total_size() const noexcept {
    size_type ret = 0;
    for (std::size_t i = 0; i < rank; ++i) {
      ret += (m_extents[i] - 1) * m_strides[i];
    }
    return ret + 1;
  }

  GSL_SUPPRESS(bounds .4) // NO-FORMAT: attribute
  constexpr size_type size() const noexcept {
    size_type ret = 1;
    for (std::size_t i = 0; i < rank; ++i) {
      ret *= m_extents[i];
    }
    return ret;
  }

  constexpr bool contains(const index_type &idx) const noexcept {
    for (std::size_t i = 0; i < rank; ++i) {
      if (idx[i] < 0 || idx[i] >= m_extents[i])
        return false;
    }
    return true;
  }

  GSL_SUPPRESS(bounds .4) // NO-FORMAT: attribute
  constexpr size_type linearize(const index_type &idx) const {
    size_type ret = 0;
    for (std::size_t i = 0; i < rank; i++) {
      Expects(idx[i] < m_extents[i]); // index is out of bounds of the array
      ret += idx[i] * m_strides[i];
    }
    return ret;
  }

  GSL_SUPPRESS(bounds .4) // NO-FORMAT: attribute
  constexpr size_type stride() const noexcept { return m_strides[0]; }

  template <bool Enabled = (rank > 1),
            typename Ret = std::enable_if_t<Enabled, sliced_type>>
  constexpr sliced_type slice() const {
    return {details::shift_left(m_extents), details::shift_left(m_strides)};
  }

  template <std::size_t Dim = 0>

  GSL_SUPPRESS(bounds .4) // NO-FORMAT: attribute
  constexpr size_type extent() const noexcept {
    static_assert(
        Dim < Rank,
        "dimension should be less than rank (dimension count starts from 0)");
    return m_extents[Dim];
  }

  constexpr index_type index_bounds() const noexcept { return m_extents; }

  constexpr const_iterator begin() const noexcept {
    return const_iterator{*this, index_type{}};
  }

  constexpr const_iterator end() const noexcept {
    return const_iterator{*this, index_bounds()};
  }

private:
  index_type m_extents;
  index_type m_strides;
};

template <typename T> struct is_bounds : std::integral_constant<bool, false> {};
template <std::ptrdiff_t... Ranges>
struct is_bounds<static_bounds<Ranges...>>
    : std::integral_constant<bool, true> {};
template <std::size_t Rank>
struct is_bounds<strided_bounds<Rank>> : std::integral_constant<bool, true> {};

template <typename IndexType> class bounds_iterator {
public:
  static const std::size_t rank = IndexType::rank;
  using iterator_category = std::random_access_iterator_tag;
  using value_type = IndexType;
  using difference_type = std::ptrdiff_t;
  using pointer = value_type *;
  using reference = value_type &;
  using index_type = value_type;
  using index_size_type = typename IndexType::value_type;
  template <typename Bounds>
  explicit bounds_iterator(const Bounds &bnd, value_type curr) noexcept
      : boundary_(bnd.index_bounds()), curr_(std::move(curr)) {
    static_assert(is_bounds<Bounds>::value, "Bounds type must be provided");
  }

  constexpr reference operator*() const noexcept { return curr_; }

  constexpr pointer operator->() const noexcept { return &curr_; }

  GSL_SUPPRESS(bounds .4) // NO-FORMAT: attribute
  GSL_SUPPRESS(bounds .2) // NO-FORMAT: attribute
  constexpr bounds_iterator &operator++() noexcept

  {
    for (std::size_t i = rank; i-- > 0;) {
      if (curr_[i] < boundary_[i] - 1) {
        curr_[i]++;
        return *this;
      }
      curr_[i] = 0;
    }
    // If we're here we've wrapped over - set to past-the-end.
    curr_ = boundary_;
    return *this;
  }

  constexpr bounds_iterator operator++(int)noexcept {
    auto ret = *this;
    ++(*this);
    return ret;
  }

  GSL_SUPPRESS(bounds .4) // NO-FORMAT: attribute
  constexpr bounds_iterator &operator--() {
    if (!less(curr_, boundary_)) {
      // if at the past-the-end, set to last element
      for (std::size_t i = 0; i < rank; ++i) {
        curr_[i] = boundary_[i] - 1;
      }
      return *this;
    }
    for (std::size_t i = rank; i-- > 0;) {
      if (curr_[i] >= 1) {
        curr_[i]--;
        return *this;
      }
      curr_[i] = boundary_[i] - 1;
    }
    // If we're here the preconditions were violated
    // "pre: there exists s such that r == ++s"
    Expects(false);
    return *this;
  }

  constexpr bounds_iterator operator--(int)noexcept {
    auto ret = *this;
    --(*this);
    return ret;
  }

  constexpr bounds_iterator operator+(difference_type n) const noexcept {
    bounds_iterator ret{*this};
    return ret += n;
  }

  GSL_SUPPRESS(bounds .4) // NO-FORMAT: attribute
  constexpr bounds_iterator &operator+=(difference_type n) {
    auto linear_idx = linearize(curr_) + n;
    std::remove_const_t<value_type> stride = 0;
    stride[rank - 1] = 1;
    for (std::size_t i = rank - 1; i-- > 0;) {
      stride[i] = stride[i + 1] * boundary_[i + 1];
    }
    for (std::size_t i = 0; i < rank; ++i) {
      curr_[i] = linear_idx / stride[i];
      linear_idx = linear_idx % stride[i];
    }
    // index is out of bounds of the array
    Expects(!less(curr_, index_type{}) && !less(boundary_, curr_));
    return *this;
  }

  constexpr bounds_iterator operator-(difference_type n) const noexcept {
    bounds_iterator ret{*this};
    return ret -= n;
  }

  constexpr bounds_iterator &operator-=(difference_type n) noexcept {
    return *this += -n;
  }

  constexpr difference_type operator-(const bounds_iterator &rhs) const
      noexcept {
    return linearize(curr_) - linearize(rhs.curr_);
  }

  constexpr value_type operator[](difference_type n) const noexcept {
    return *(*this + n);
  }

  constexpr bool operator==(const bounds_iterator &rhs) const noexcept {
    return curr_ == rhs.curr_;
  }

  constexpr bool operator!=(const bounds_iterator &rhs) const noexcept {
    return !(*this == rhs);
  }

  constexpr bool operator<(const bounds_iterator &rhs) const noexcept {
    return less(curr_, rhs.curr_);
  }

  constexpr bool operator<=(const bounds_iterator &rhs) const noexcept {
    return !(rhs < *this);
  }

  constexpr bool operator>(const bounds_iterator &rhs) const noexcept {
    return rhs < *this;
  }

  constexpr bool operator>=(const bounds_iterator &rhs) const noexcept {
    return !(rhs > *this);
  }

  void swap(bounds_iterator &rhs) noexcept {
    std::swap(boundary_, rhs.boundary_);
    std::swap(curr_, rhs.curr_);
  }

private:
  GSL_SUPPRESS(bounds .4) // NO-FORMAT: attribute
  constexpr bool less(index_type &one, index_type &other) const noexcept {
    for (std::size_t i = 0; i < rank; ++i) {
      if (one[i] < other[i])
        return true;
    }
    return false;
  }

  GSL_SUPPRESS(bounds .4) // NO-FORMAT: attribute
  constexpr index_size_type linearize(const value_type &idx) const noexcept {
    // TODO: Smarter impl.
    // Check if past-the-end
    index_size_type multiplier = 1;
    index_size_type res = 0;
    if (!less(idx, boundary_)) {
      res = 1;
      for (std::size_t i = rank; i-- > 0;) {
        res += (idx[i] - 1) * multiplier;
        multiplier *= boundary_[i];
      }
    } else {
      for (std::size_t i = rank; i-- > 0;) {
        res += idx[i] * multiplier;
        multiplier *= boundary_[i];
      }
    }
    return res;
  }

  value_type boundary_;
  std::remove_const_t<value_type> curr_;
};

template <typename IndexType>
bounds_iterator<IndexType>
operator+(typename bounds_iterator<IndexType>::difference_type n,
          const bounds_iterator<IndexType> &rhs) noexcept {
  return rhs + n;
}

namespace details {
template <typename Bounds>
constexpr std::enable_if_t<
    std::is_same<typename Bounds::mapping_type, generalized_mapping_tag>::value,
    typename Bounds::index_type>
make_stride(const Bounds &bnd) noexcept {
  return bnd.strides();
}

// Make a stride vector from bounds, assuming contiguous memory.
template <typename Bounds>
constexpr std::enable_if_t<
    std::is_same<typename Bounds::mapping_type, contiguous_mapping_tag>::value,
    typename Bounds::index_type>
make_stride(const Bounds &bnd) noexcept {
  auto extents = bnd.index_bounds();
  typename Bounds::size_type stride[Bounds::rank] = {};

  stride[Bounds::rank - 1] = 1;
  for (std::size_t i = 1; i < Bounds::rank; ++i) {
    GSL_SUPPRESS(bounds .4) // NO-FORMAT: attribute
    GSL_SUPPRESS(bounds .2) // NO-FORMAT: attribute
    stride[Bounds::rank - i - 1] =
        stride[Bounds::rank - i] * extents[Bounds::rank - i];
  }
  return {stride};
}

template <typename BoundsSrc, typename BoundsDest>
void verifyBoundsReshape(const BoundsSrc &src, const BoundsDest &dest) {
  static_assert(is_bounds<BoundsSrc>::value && is_bounds<BoundsDest>::value,
                "The src type and dest type must be bounds");
  static_assert(std::is_same<typename BoundsSrc::mapping_type,
                             contiguous_mapping_tag>::value,
                "The source type must be a contiguous bounds");
  static_assert(BoundsDest::static_size == dynamic_range ||
                    BoundsSrc::static_size == dynamic_range ||
                    BoundsDest::static_size == BoundsSrc::static_size,
                "The source bounds must have same size as dest bounds");
  Expects(src.size() == dest.size());
}

} // namespace details

template <typename Span> class contiguous_span_iterator;
template <typename Span> class general_span_iterator;

template <std::ptrdiff_t DimSize = dynamic_range> struct dim_t {
  static const std::ptrdiff_t value = DimSize;
};
template <> struct dim_t<dynamic_range> {
  static const std::ptrdiff_t value = dynamic_range;
  const std::ptrdiff_t dvalue;
  constexpr dim_t(std::ptrdiff_t size) noexcept : dvalue(size) {}
};

template <std::ptrdiff_t N, class = std::enable_if_t<(N >= 0)>>
constexpr dim_t<N> dim() noexcept {
  return dim_t<N>();
}

template <std::ptrdiff_t N = dynamic_range,
          class = std::enable_if_t<N == dynamic_range>>
constexpr dim_t<N> dim(std::ptrdiff_t n) noexcept {
  return dim_t<>(n);
}

template <typename ValueType, std::ptrdiff_t FirstDimension = dynamic_range,
          std::ptrdiff_t... RestDimensions>
class multi_span;
template <typename ValueType, std::size_t Rank> class strided_span;

namespace details {
template <typename T, typename = std::true_type> struct SpanTypeTraits {
  using value_type = T;
  using size_type = std::size_t;
};

template <typename Traits>
struct SpanTypeTraits<
    Traits, typename std::is_reference<typename Traits::span_traits &>::type> {
  using value_type = typename Traits::span_traits::value_type;
  using size_type = typename Traits::span_traits::size_type;
};

template <typename T, std::ptrdiff_t... Ranks> struct SpanArrayTraits {
  using type = multi_span<T, Ranks...>;
  using value_type = T;
  using bounds_type = static_bounds<Ranks...>;
  using pointer = T *;
  using reference = T &;
};
template <typename T, std::ptrdiff_t N, std::ptrdiff_t... Ranks>
struct SpanArrayTraits<T[N], Ranks...> : SpanArrayTraits<T, Ranks..., N> {};

template <typename BoundsType>
BoundsType newBoundsHelperImpl(std::ptrdiff_t totalSize,
                               std::true_type) // dynamic size
{
  Expects(totalSize >= 0 && totalSize <= PTRDIFF_MAX);
  return BoundsType{totalSize};
}
template <typename BoundsType>
BoundsType newBoundsHelperImpl(std::ptrdiff_t totalSize,
                               std::false_type) // static size
{
  Expects(BoundsType::static_size <= totalSize);
  return {};
}
template <typename BoundsType>
BoundsType newBoundsHelper(std::ptrdiff_t totalSize) {
  static_assert(BoundsType::dynamic_rank <= 1,
                "dynamic rank must less or equal to 1");
  return newBoundsHelperImpl<BoundsType>(
      totalSize, std::integral_constant<bool, BoundsType::dynamic_rank == 1>());
}

struct Sep {};

template <typename T, typename... Args>
T static_as_multi_span_helper(Sep, Args... args) {
  return T{narrow_cast<typename T::size_type>(args)...};
}
template <typename T, typename Arg, typename... Args>
std::enable_if_t<!std::is_same<Arg, dim_t<dynamic_range>>::value &&
                     !std::is_same<Arg, Sep>::value,
                 T>
static_as_multi_span_helper(Arg, Args... args) {
  return static_as_multi_span_helper<T>(args...);
}
template <typename T, typename... Args>
T static_as_multi_span_helper(dim_t<dynamic_range> val, Args... args) {
  return static_as_multi_span_helper<T>(args..., val.dvalue);
}

template <typename... Dimensions>
struct static_as_multi_span_static_bounds_helper {
  using type = static_bounds<(Dimensions::value)...>;
};

template <typename T> struct is_multi_span_oracle : std::false_type {};

template <typename ValueType, std::ptrdiff_t FirstDimension,
          std::ptrdiff_t... RestDimensions>
struct is_multi_span_oracle<
    multi_span<ValueType, FirstDimension, RestDimensions...>> : std::true_type {
};

template <typename ValueType, std::ptrdiff_t Rank>
struct is_multi_span_oracle<strided_span<ValueType, Rank>> : std::true_type {};

template <typename T>
struct is_multi_span : is_multi_span_oracle<std::remove_cv_t<T>> {};
} // namespace details

template <typename ValueType, std::ptrdiff_t FirstDimension,
          std::ptrdiff_t... RestDimensions>
class multi_span {
  // TODO do we still need this?
  template <typename ValueType2, std::ptrdiff_t FirstDimension2,
            std::ptrdiff_t... RestDimensions2>
  friend class multi_span;

public:
  using bounds_type = static_bounds<FirstDimension, RestDimensions...>;
  static const std::size_t Rank = bounds_type::rank;
  using size_type = typename bounds_type::size_type;
  using index_type = typename bounds_type::index_type;
  using value_type = ValueType;
  using const_value_type = std::add_const_t<value_type>;
  using pointer = std::add_pointer_t<value_type>;
  using reference = std::add_lvalue_reference_t<value_type>;
  using iterator = contiguous_span_iterator<multi_span>;
  using const_span =
      multi_span<const_value_type, FirstDimension, RestDimensions...>;
  using const_iterator = contiguous_span_iterator<const_span>;
  using reverse_iterator = std::reverse_iterator<iterator>;
  using const_reverse_iterator = std::reverse_iterator<const_iterator>;
  using sliced_type =
      std::conditional_t<Rank == 1, value_type,
                         multi_span<value_type, RestDimensions...>>;

private:
  pointer data_;
  bounds_type bounds_;

  friend iterator;
  friend const_iterator;

public:
  // default constructor - same as constructing from nullptr_t
  GSL_SUPPRESS(type .6) // NO-FORMAT: attribute // TODO: false positive
  constexpr multi_span() noexcept : multi_span(nullptr, bounds_type{}) {
    static_assert(
        bounds_type::dynamic_rank != 0 ||
            (bounds_type::dynamic_rank == 0 && bounds_type::static_size == 0),
        "Default construction of multi_span<T> only possible "
        "for dynamic or fixed, zero-length spans.");
  }

  // construct from nullptr - get an empty multi_span
  GSL_SUPPRESS(type .6) // NO-FORMAT: attribute // TODO: false positive
  constexpr multi_span(std::nullptr_t) noexcept
      : multi_span(nullptr, bounds_type{}) {
    static_assert(
        bounds_type::dynamic_rank != 0 ||
            (bounds_type::dynamic_rank == 0 && bounds_type::static_size == 0),
        "nullptr_t construction of multi_span<T> only possible "
        "for dynamic or fixed, zero-length spans.");
  }

  // construct from nullptr with size of 0 (helps with template function calls)
  template <class IntType,
            typename = std::enable_if_t<std::is_integral<IntType>::value>>

  // GSL_SUPPRESS(type.6) // NO-FORMAT: attribute // TODO: false positive //
  // TODO: parser bug
  constexpr multi_span(std::nullptr_t, IntType size)
      : multi_span(nullptr, bounds_type{}) {
    static_assert(
        bounds_type::dynamic_rank != 0 ||
            (bounds_type::dynamic_rank == 0 && bounds_type::static_size == 0),
        "nullptr_t construction of multi_span<T> only possible "
        "for dynamic or fixed, zero-length spans.");
    Expects(size == 0);
  }

  // construct from a single element

  GSL_SUPPRESS(type .6) // NO-FORMAT: attribute // TODO: false positive
  constexpr multi_span(reference data) noexcept
      : multi_span(&data, bounds_type{1}) {
    static_assert(bounds_type::dynamic_rank > 0 ||
                      bounds_type::static_size == 0 ||
                      bounds_type::static_size == 1,
                  "Construction from a single element only possible "
                  "for dynamic or fixed spans of length 0 or 1.");
  }

  // prevent constructing from temporaries for single-elements
  constexpr multi_span(value_type &&) = delete;

  // construct from pointer + length
  GSL_SUPPRESS(type .6) // NO-FORMAT: attribute // TODO: false positive
  constexpr multi_span(pointer ptr, size_type size)
      : multi_span(ptr, bounds_type{size}) {}

  // construct from pointer + length - multidimensional
  constexpr multi_span(pointer data, bounds_type bounds)
      : data_(data), bounds_(std::move(bounds)) {
    Expects((bounds_.size() > 0 && data != nullptr) || bounds_.size() == 0);
  }

  // construct from begin,end pointer pair
  template <typename Ptr,
            typename = std::enable_if_t<
                std::is_convertible<Ptr, pointer>::value &&
                details::LessThan<bounds_type::dynamic_rank, 2>::value>>
  constexpr multi_span(pointer begin, Ptr end)
      : multi_span(begin, details::newBoundsHelper<bounds_type>(
                              static_cast<pointer>(end) - begin)) {
    Expects(begin != nullptr && end != nullptr &&
            begin <= static_cast<pointer>(end));
  }

  // construct from n-dimensions static array
  template <typename T, std::size_t N,
            typename Helper = details::SpanArrayTraits<T, N>>
  constexpr multi_span(T (&arr)[N])
      : multi_span(reinterpret_cast<pointer>(arr),
                   bounds_type{typename Helper::bounds_type{}}) {
    static_assert(std::is_convertible<typename Helper::value_type(*)[],
                                      value_type(*)[]>::value,
                  "Cannot convert from source type to target multi_span type.");
    static_assert(
        std::is_convertible<typename Helper::bounds_type, bounds_type>::value,
        "Cannot construct a multi_span from an array with fewer elements.");
  }

  // construct from n-dimensions dynamic array (e.g. new int[m][4])
  // (precedence will be lower than the 1-dimension pointer)
  template <typename T,
            typename Helper = details::SpanArrayTraits<T, dynamic_range>>
  constexpr multi_span(T *const &data, size_type size)
      : multi_span(reinterpret_cast<pointer>(data),
                   typename Helper::bounds_type{size}) {
    static_assert(std::is_convertible<typename Helper::value_type(*)[],
                                      value_type(*)[]>::value,
                  "Cannot convert from source type to target multi_span type.");
  }

  // construct from std::array
  template <typename T, std::size_t N>
  constexpr multi_span(std::array<T, N> &arr)
      : multi_span(arr.data(), bounds_type{static_bounds<N>{}}) {
    static_assert(
        std::is_convertible<
            T(*)[], typename std::remove_const_t<value_type>(*)[]>::value,
        "Cannot convert from source type to target multi_span type.");
    static_assert(
        std::is_convertible<static_bounds<N>, bounds_type>::value,
        "You cannot construct a multi_span from a std::array of smaller size.");
  }

  // construct from const std::array
  template <typename T, std::size_t N>
  constexpr multi_span(const std::array<T, N> &arr)
      : multi_span(arr.data(), bounds_type{static_bounds<N>{}}) {
    static_assert(
        std::is_convertible<
            T(*)[], typename std::remove_const_t<value_type>(*)[]>::value,
        "Cannot convert from source type to target multi_span type.");
    static_assert(
        std::is_convertible<static_bounds<N>, bounds_type>::value,
        "You cannot construct a multi_span from a std::array of smaller size.");
  }

  // prevent constructing from temporary std::array
  template <typename T, std::size_t N>
  constexpr multi_span(std::array<T, N> &&arr) = delete;

  // construct from containers
  // future: could use contiguous_iterator_traits to identify only contiguous
  // containers type-requirements: container must have .size(), operator[] which
  // are value_type compatible
  template <
      typename Cont, typename DataType = typename Cont::value_type,
      typename = std::enable_if_t<
          !details::is_multi_span<Cont>::value &&
          std::is_convertible<DataType (*)[], value_type (*)[]>::value &&
          std::is_same<std::decay_t<decltype(std::declval<Cont>().size(),
                                             *std::declval<Cont>().data())>,
                       DataType>::value>>
  constexpr multi_span(Cont &cont)
      : multi_span(static_cast<pointer>(cont.data()),
                   details::newBoundsHelper<bounds_type>(
                       narrow_cast<size_type>(cont.size()))) {}

  // prevent constructing from temporary containers
  template <
      typename Cont, typename DataType = typename Cont::value_type,
      typename = std::enable_if_t<
          !details::is_multi_span<Cont>::value &&
          std::is_convertible<DataType (*)[], value_type (*)[]>::value &&
          std::is_same<std::decay_t<decltype(std::declval<Cont>().size(),
                                             *std::declval<Cont>().data())>,
                       DataType>::value>>
  explicit constexpr multi_span(Cont &&cont) = delete;

  // construct from a convertible multi_span
  template <typename OtherValueType, std::ptrdiff_t... OtherDimensions,
            typename OtherBounds = static_bounds<OtherDimensions...>,
            typename = std::enable_if_t<
                std::is_convertible<OtherValueType, ValueType>::value &&
                std::is_convertible<OtherBounds, bounds_type>::value>>
  constexpr multi_span(multi_span<OtherValueType, OtherDimensions...> other)
      : data_(other.data_), bounds_(other.bounds_) {}

  // trivial copy and move
  constexpr multi_span(const multi_span &) = default;
  constexpr multi_span(multi_span &&) = default;

  // trivial assignment
  constexpr multi_span &operator=(const multi_span &) = default;
  constexpr multi_span &operator=(multi_span &&) = default;

  // first() - extract the first Count elements into a new multi_span
  template <std::ptrdiff_t Count>

  constexpr multi_span<ValueType, Count> first() const {
    static_assert(Count >= 0, "Count must be >= 0.");
    static_assert(bounds_type::static_size == dynamic_range ||
                      Count <= bounds_type::static_size,
                  "Count is out of bounds.");

    Expects(bounds_type::static_size != dynamic_range || Count <= this->size());
    return {this->data(), Count};
  }

  // first() - extract the first count elements into a new multi_span
  constexpr multi_span<ValueType, dynamic_range> first(size_type count) const {
    Expects(count >= 0 && count <= this->size());
    return {this->data(), count};
  }

  // last() - extract the last Count elements into a new multi_span
  template <std::ptrdiff_t Count>
  constexpr multi_span<ValueType, Count> last() const {
    static_assert(Count >= 0, "Count must be >= 0.");
    static_assert(bounds_type::static_size == dynamic_range ||
                      Count <= bounds_type::static_size,
                  "Count is out of bounds.");

    Expects(bounds_type::static_size != dynamic_range || Count <= this->size());
    return {this->data() + this->size() - Count, Count};
  }

  // last() - extract the last count elements into a new multi_span
  constexpr multi_span<ValueType, dynamic_range> last(size_type count) const {
    Expects(count >= 0 && count <= this->size());
    return {this->data() + this->size() - count, count};
  }

  // subspan() - create a subview of Count elements starting at Offset
  template <std::ptrdiff_t Offset, std::ptrdiff_t Count>
  constexpr multi_span<ValueType, Count> subspan() const {
    static_assert(Count >= 0, "Count must be >= 0.");
    static_assert(Offset >= 0, "Offset must be >= 0.");
    static_assert(
        bounds_type::static_size == dynamic_range ||
            ((Offset <= bounds_type::static_size) &&
             Count <= bounds_type::static_size - Offset),
        "You must describe a sub-range within bounds of the multi_span.");

    Expects(bounds_type::static_size != dynamic_range ||
            (Offset <= this->size() && Count <= this->size() - Offset));
    return {this->data() + Offset, Count};
  }

  // subspan() - create a subview of count elements starting at offset
  // supplying dynamic_range for count will consume all available elements from
  // offset
  constexpr multi_span<ValueType, dynamic_range>
  subspan(size_type offset, size_type count = dynamic_range) const {
    Expects((offset >= 0 && offset <= this->size()) &&
            (count == dynamic_range || (count <= this->size() - offset)));
    return {this->data() + offset,
            count == dynamic_range ? this->length() - offset : count};
  }

  // section - creates a non-contiguous, strided multi_span from a contiguous
  // one
  constexpr strided_span<ValueType, Rank> section(index_type origin,
                                                  index_type extents) const {
    const size_type size =
        this->bounds().total_size() - this->bounds().linearize(origin);
    return {&this->operator[](origin), size,
            strided_bounds<Rank>{extents, details::make_stride(bounds())}};
  }

  // length of the multi_span in elements
  constexpr size_type size() const noexcept { return bounds_.size(); }

  // length of the multi_span in elements
  constexpr size_type length() const noexcept { return this->size(); }

  // length of the multi_span in bytes
  constexpr size_type size_bytes() const noexcept {
    return narrow_cast<size_type>(sizeof(value_type)) * this->size();
  }

  // length of the multi_span in bytes
  constexpr size_type length_bytes() const noexcept {
    return this->size_bytes();
  }

  constexpr bool empty() const noexcept { return this->size() == 0; }

  static constexpr std::size_t rank() { return Rank; }

  template <std::size_t Dim = 0> constexpr size_type extent() const noexcept {
    static_assert(
        Dim < Rank,
        "Dimension should be less than rank (dimension count starts from 0).");
    return bounds_.template extent<Dim>();
  }

  template <typename IntType> constexpr size_type extent(IntType dim) const {
    return bounds_.extent(dim);
  }

  constexpr bounds_type bounds() const noexcept { return bounds_; }

  constexpr pointer data() const noexcept { return data_; }

  template <typename FirstIndex>
  constexpr reference operator()(FirstIndex idx) {
    return this->operator[](narrow_cast<std::ptrdiff_t>(idx));
  }

  template <typename FirstIndex, typename... OtherIndices>
  constexpr reference operator()(FirstIndex firstIndex,
                                 OtherIndices... indices) {
    const index_type idx = {narrow_cast<std::ptrdiff_t>(firstIndex),
                            narrow_cast<std::ptrdiff_t>(indices)...};
    return this->operator[](idx);
  }

  GSL_SUPPRESS(bounds .1) // NO-FORMAT: attribute
  constexpr reference operator[](const index_type &idx) const {
    return data_[bounds_.linearize(idx)];
  }

  template <bool Enabled = (Rank > 1),
            typename Ret = std::enable_if_t<Enabled, sliced_type>>

  GSL_SUPPRESS(bounds .1) // NO-FORMAT: attribute
  constexpr Ret operator[](size_type idx) const {
    Expects(idx >= 0 &&
            idx < bounds_.size()); // index is out of bounds of the array
    const size_type ridx = idx * bounds_.stride();

    // index is out of bounds of the underlying data
    Expects(ridx < bounds_.total_size());
    return Ret{data_ + ridx, bounds_.slice()};
  }

  constexpr iterator begin() const noexcept { return iterator{this, true}; }

  constexpr iterator end() const noexcept { return iterator{this, false}; }

  GSL_SUPPRESS(type .1) // NO-FORMAT: attribute
  constexpr const_iterator cbegin() const noexcept {
    return const_iterator{reinterpret_cast<const const_span *>(this), true};
  }

  constexpr const_iterator cend() const noexcept {
    return const_iterator{reinterpret_cast<const const_span *>(this), false};
  }

  constexpr reverse_iterator rbegin() const noexcept {
    return reverse_iterator{end()};
  }

  constexpr reverse_iterator rend() const noexcept {
    return reverse_iterator{begin()};
  }

  constexpr const_reverse_iterator crbegin() const noexcept {
    return const_reverse_iterator{cend()};
  }

  constexpr const_reverse_iterator crend() const noexcept {
    return const_reverse_iterator{cbegin()};
  }

  template <typename OtherValueType, std::ptrdiff_t... OtherDimensions,
            typename = std::enable_if_t<
                std::is_same<std::remove_cv_t<value_type>,
                             std::remove_cv_t<OtherValueType>>::value>>
  constexpr bool operator==(
      const multi_span<OtherValueType, OtherDimensions...> &other) const {
    return bounds_.size() == other.bounds_.size() &&
           (data_ == other.data_ ||
            std::equal(this->begin(), this->end(), other.begin()));
  }

  template <typename OtherValueType, std::ptrdiff_t... OtherDimensions,
            typename = std::enable_if_t<
                std::is_same<std::remove_cv_t<value_type>,
                             std::remove_cv_t<OtherValueType>>::value>>
  constexpr bool operator!=(
      const multi_span<OtherValueType, OtherDimensions...> &other) const {
    return !(*this == other);
  }

  template <typename OtherValueType, std::ptrdiff_t... OtherDimensions,
            typename = std::enable_if_t<
                std::is_same<std::remove_cv_t<value_type>,
                             std::remove_cv_t<OtherValueType>>::value>>
  constexpr bool
  operator<(const multi_span<OtherValueType, OtherDimensions...> &other) const {
    return std::lexicographical_compare(this->begin(), this->end(),
                                        other.begin(), other.end());
  }

  template <typename OtherValueType, std::ptrdiff_t... OtherDimensions,
            typename = std::enable_if_t<
                std::is_same<std::remove_cv_t<value_type>,
                             std::remove_cv_t<OtherValueType>>::value>>
  constexpr bool operator<=(
      const multi_span<OtherValueType, OtherDimensions...> &other) const {
    return !(other < *this);
  }

  template <typename OtherValueType, std::ptrdiff_t... OtherDimensions,
            typename = std::enable_if_t<
                std::is_same<std::remove_cv_t<value_type>,
                             std::remove_cv_t<OtherValueType>>::value>>
  constexpr bool
  operator>(const multi_span<OtherValueType, OtherDimensions...> &other) const
      noexcept {
    return (other < *this);
  }

  template <typename OtherValueType, std::ptrdiff_t... OtherDimensions,
            typename = std::enable_if_t<
                std::is_same<std::remove_cv_t<value_type>,
                             std::remove_cv_t<OtherValueType>>::value>>
  constexpr bool operator>=(
      const multi_span<OtherValueType, OtherDimensions...> &other) const {
    return !(*this < other);
  }
};

//
// Free functions for manipulating spans
//

// reshape a multi_span into a different dimensionality
// DimCount and Enabled here are workarounds for a bug in MSVC 2015
template <typename SpanType, typename... Dimensions2,
          std::size_t DimCount = sizeof...(Dimensions2),
          bool Enabled = (DimCount > 0), typename = std::enable_if_t<Enabled>>
constexpr auto as_multi_span(SpanType s, Dimensions2... dims)
    -> multi_span<typename SpanType::value_type, Dimensions2::value...> {
  static_assert(details::is_multi_span<SpanType>::value,
                "Variadic as_multi_span() is for reshaping existing spans.");
  using BoundsType = typename multi_span<typename SpanType::value_type,
                                         (Dimensions2::value)...>::bounds_type;
  const auto tobounds =
      details::static_as_multi_span_helper<BoundsType>(dims..., details::Sep{});
  details::verifyBoundsReshape(s.bounds(), tobounds);
  return {s.data(), tobounds};
}

// convert a multi_span<T> to a multi_span<const byte>
template <typename U, std::ptrdiff_t... Dimensions>
multi_span<const byte, dynamic_range>
as_bytes(multi_span<U, Dimensions...> s) noexcept {
  static_assert(std::is_trivial<std::decay_t<U>>::value,
                "The value_type of multi_span must be a trivial type.");
  return {reinterpret_cast<const byte *>(s.data()), s.size_bytes()};
}

// convert a multi_span<T> to a multi_span<byte> (a writeable byte multi_span)
// this is not currently a portable function that can be relied upon to work
// on all implementations. It should be considered an experimental extension
// to the standard GSL interface.
template <typename U, std::ptrdiff_t... Dimensions>
multi_span<byte> as_writeable_bytes(multi_span<U, Dimensions...> s) noexcept {
  static_assert(std::is_trivial<std::decay_t<U>>::value,
                "The value_type of multi_span must be a trivial type.");
  return {reinterpret_cast<byte *>(s.data()), s.size_bytes()};
}

// convert a multi_span<const byte> to a multi_span<const T>
// this is not currently a portable function that can be relied upon to work
// on all implementations. It should be considered an experimental extension
// to the standard GSL interface.
template <typename U, std::ptrdiff_t... Dimensions>
constexpr auto
as_multi_span(multi_span<const byte, Dimensions...> s) -> multi_span<
    const U,
    static_cast<std::ptrdiff_t>(
        multi_span<const byte, Dimensions...>::bounds_type::static_size !=
                dynamic_range
            ? (static_cast<std::size_t>(multi_span<const byte, Dimensions...>::
                                            bounds_type::static_size) /
               sizeof(U))
            : dynamic_range)> {
  using ConstByteSpan = multi_span<const byte, Dimensions...>;
  static_assert(std::is_trivial<std::decay_t<U>>::value &&
                    (ConstByteSpan::bounds_type::static_size == dynamic_range ||
                     ConstByteSpan::bounds_type::static_size %
                             narrow_cast<std::ptrdiff_t>(sizeof(U)) ==
                         0),
                "Target type must be a trivial type and its size must match "
                "the byte array size");

  Expects((s.size_bytes() % narrow_cast<std::ptrdiff_t>(sizeof(U))) == 0 &&
          (s.size_bytes() / narrow_cast<std::ptrdiff_t>(sizeof(U))) <
              PTRDIFF_MAX);
  return {reinterpret_cast<const U *>(s.data()),
          s.size_bytes() / narrow_cast<std::ptrdiff_t>(sizeof(U))};
}

// convert a multi_span<byte> to a multi_span<T>
// this is not currently a portable function that can be relied upon to work
// on all implementations. It should be considered an experimental extension
// to the standard GSL interface.
template <typename U, std::ptrdiff_t... Dimensions>
constexpr auto as_multi_span(multi_span<byte, Dimensions...> s) -> multi_span<
    U,
    narrow_cast<std::ptrdiff_t>(
        multi_span<byte, Dimensions...>::bounds_type::static_size !=
                dynamic_range
            ? static_cast<std::size_t>(
                  multi_span<byte, Dimensions...>::bounds_type::static_size) /
                  sizeof(U)
            : dynamic_range)> {
  using ByteSpan = multi_span<byte, Dimensions...>;
  static_assert(std::is_trivial<std::decay_t<U>>::value &&
                    (ByteSpan::bounds_type::static_size == dynamic_range ||
                     ByteSpan::bounds_type::static_size % sizeof(U) == 0),
                "Target type must be a trivial type and its size must match "
                "the byte array size");

  Expects((s.size_bytes() % sizeof(U)) == 0);
  return {reinterpret_cast<U *>(s.data()),
          s.size_bytes() / narrow_cast<std::ptrdiff_t>(sizeof(U))};
}

template <typename T, std::ptrdiff_t... Dimensions>
constexpr auto as_multi_span(T *const &ptr, dim_t<Dimensions>... args)
    -> multi_span<std::remove_all_extents_t<T>, Dimensions...> {
  return {reinterpret_cast<std::remove_all_extents_t<T> *>(ptr),
          details::static_as_multi_span_helper<static_bounds<Dimensions...>>(
              args..., details::Sep{})};
}

template <typename T>
constexpr auto as_multi_span(T *arr, std::ptrdiff_t len) ->
    typename details::SpanArrayTraits<T, dynamic_range>::type {
  return {reinterpret_cast<std::remove_all_extents_t<T> *>(arr), len};
}

template <typename T, std::size_t N>
constexpr auto as_multi_span(T (&arr)[N]) ->
    typename details::SpanArrayTraits<T, N>::type {
  return {arr};
}

template <typename T, std::size_t N>
constexpr multi_span<const T, N> as_multi_span(const std::array<T, N> &arr) {
  return {arr};
}

template <typename T, std::size_t N>
constexpr multi_span<const T, N>
as_multi_span(const std::array<T, N> &&) = delete;

template <typename T, std::size_t N>
constexpr multi_span<T, N> as_multi_span(std::array<T, N> &arr) {
  return {arr};
}

template <typename T>
constexpr multi_span<T, dynamic_range> as_multi_span(T *begin, T *end) {
  return {begin, end};
}

template <typename Cont>
constexpr auto as_multi_span(Cont &arr) -> std::enable_if_t<
    !details::is_multi_span<std::decay_t<Cont>>::value,
    multi_span<std::remove_reference_t<decltype(arr.size(), *arr.data())>,
               dynamic_range>> {
  Expects(arr.size() < PTRDIFF_MAX);
  return {arr.data(), narrow_cast<std::ptrdiff_t>(arr.size())};
}

template <typename Cont>
constexpr auto as_multi_span(Cont &&arr) -> std::enable_if_t<
    !details::is_multi_span<std::decay_t<Cont>>::value,
    multi_span<std::remove_reference_t<decltype(arr.size(), *arr.data())>,
               dynamic_range>> = delete;

// from basic_string which doesn't have nonconst .data() member like other
// contiguous containers
template <typename CharT, typename Traits, typename Allocator>
GSL_SUPPRESS(bounds .4) // NO-FORMAT: attribute
constexpr auto as_multi_span(std::basic_string<CharT, Traits, Allocator> &str)
    -> multi_span<CharT, dynamic_range> {
  Expects(str.size() < PTRDIFF_MAX);
  return {&str[0], narrow_cast<std::ptrdiff_t>(str.size())};
}

// strided_span is an extension that is not strictly part of the GSL at this
// time. It is kept here while the multidimensional interface is still being
// defined.
template <typename ValueType, std::size_t Rank> class strided_span {
public:
  using bounds_type = strided_bounds<Rank>;
  using size_type = typename bounds_type::size_type;
  using index_type = typename bounds_type::index_type;
  using value_type = ValueType;
  using const_value_type = std::add_const_t<value_type>;
  using pointer = std::add_pointer_t<value_type>;
  using reference = std::add_lvalue_reference_t<value_type>;
  using iterator = general_span_iterator<strided_span>;
  using const_strided_span = strided_span<const_value_type, Rank>;
  using const_iterator = general_span_iterator<const_strided_span>;
  using reverse_iterator = std::reverse_iterator<iterator>;
  using const_reverse_iterator = std::reverse_iterator<const_iterator>;
  using sliced_type = std::conditional_t<Rank == 1, value_type,
                                         strided_span<value_type, Rank - 1>>;

private:
  pointer data_;
  bounds_type bounds_;

  friend iterator;
  friend const_iterator;
  template <typename OtherValueType, std::size_t OtherRank>
  friend class strided_span;

public:
  // from raw data
  constexpr strided_span(pointer ptr, size_type size, bounds_type bounds)
      : data_(ptr), bounds_(std::move(bounds)) {
    Expects((bounds_.size() > 0 && ptr != nullptr) || bounds_.size() == 0);
    // Bounds cross data boundaries
    Expects(this->bounds().total_size() <= size);
    GSL_SUPPRESS(type .4) // NO-FORMAT: attribute // TODO: false positive
    (void)size;
  }

  // from static array of size N
  template <size_type N>
  constexpr strided_span(value_type (&values)[N], bounds_type bounds)
      : strided_span(values, N, std::move(bounds)) {}

  // from array view
  template <
      typename OtherValueType, std::ptrdiff_t... Dimensions,
      bool Enabled1 = (sizeof...(Dimensions) == Rank),
      bool Enabled2 = std::is_convertible<OtherValueType *, ValueType *>::value,
      typename = std::enable_if_t<Enabled1 && Enabled2>>
  constexpr strided_span(multi_span<OtherValueType, Dimensions...> av,
                         bounds_type bounds)
      : strided_span(av.data(), av.bounds().total_size(), std::move(bounds)) {}

  // convertible
  template <typename OtherValueType,
            typename = std::enable_if_t<std::is_convertible<
                OtherValueType (*)[], value_type (*)[]>::value>>
  constexpr strided_span(const strided_span<OtherValueType, Rank> &other)
      : data_(other.data_), bounds_(other.bounds_) {}

  // convert from bytes
  template <typename OtherValueType>
  constexpr strided_span<
      typename std::enable_if<std::is_same<value_type, const byte>::value,
                              OtherValueType>::type,
      Rank>
  as_strided_span() const {
    static_assert((sizeof(OtherValueType) >= sizeof(value_type)) &&
                      (sizeof(OtherValueType) % sizeof(value_type) == 0),
                  "OtherValueType should have a size to contain a multiple of "
                  "ValueTypes");
    auto d =
        narrow_cast<size_type>(sizeof(OtherValueType) / sizeof(value_type));

    const size_type size = this->bounds().total_size() / d;

    GSL_SUPPRESS(type .3) // NO-FORMAT: attribute
    return {const_cast<OtherValueType *>(
                reinterpret_cast<const OtherValueType *>(this->data())),
            size,
            bounds_type{resize_extent(this->bounds().index_bounds(), d),
                        resize_stride(this->bounds().strides(), d)}};
  }

  constexpr strided_span section(index_type origin, index_type extents) const {
    const size_type size =
        this->bounds().total_size() - this->bounds().linearize(origin);
    return {&this->operator[](origin), size,
            bounds_type{extents, details::make_stride(bounds())}};
  }

  GSL_SUPPRESS(bounds .1) // NO-FORMAT: attribute
  constexpr reference operator[](const index_type &idx) const {
    return data_[bounds_.linearize(idx)];
  }

  template <bool Enabled = (Rank > 1),
            typename Ret = std::enable_if_t<Enabled, sliced_type>>
  GSL_SUPPRESS(bounds .1) // NO-FORMAT: attribute
  constexpr Ret operator[](size_type idx) const {
    Expects(idx < bounds_.size()); // index is out of bounds of the array
    const size_type ridx = idx * bounds_.stride();

    // index is out of bounds of the underlying data
    Expects(ridx < bounds_.total_size());
    return {data_ + ridx, bounds_.slice().total_size(), bounds_.slice()};
  }

  constexpr bounds_type bounds() const noexcept { return bounds_; }

  template <std::size_t Dim = 0> constexpr size_type extent() const noexcept {
    static_assert(
        Dim < Rank,
        "dimension should be less than Rank (dimension count starts from 0)");
    return bounds_.template extent<Dim>();
  }

  constexpr size_type size() const noexcept { return bounds_.size(); }

  constexpr pointer data() const noexcept { return data_; }

  constexpr bool empty() const noexcept { return this->size() == 0; }

  constexpr explicit operator bool() const noexcept { return data_ != nullptr; }

  constexpr iterator begin() const { return iterator{this, true}; }

  constexpr iterator end() const { return iterator{this, false}; }

  constexpr const_iterator cbegin() const {
    return const_iterator{reinterpret_cast<const const_strided_span *>(this),
                          true};
  }

  constexpr const_iterator cend() const {
    return const_iterator{reinterpret_cast<const const_strided_span *>(this),
                          false};
  }

  constexpr reverse_iterator rbegin() const { return reverse_iterator{end()}; }

  constexpr reverse_iterator rend() const { return reverse_iterator{begin()}; }

  constexpr const_reverse_iterator crbegin() const {
    return const_reverse_iterator{cend()};
  }

  constexpr const_reverse_iterator crend() const {
    return const_reverse_iterator{cbegin()};
  }

  template <typename OtherValueType, std::ptrdiff_t OtherRank,
            typename = std::enable_if_t<
                std::is_same<std::remove_cv_t<value_type>,
                             std::remove_cv_t<OtherValueType>>::value>>
  constexpr bool
  operator==(const strided_span<OtherValueType, OtherRank> &other) const {
    return bounds_.size() == other.bounds_.size() &&
           (data_ == other.data_ ||
            std::equal(this->begin(), this->end(), other.begin()));
  }

  template <typename OtherValueType, std::ptrdiff_t OtherRank,
            typename = std::enable_if_t<
                std::is_same<std::remove_cv_t<value_type>,
                             std::remove_cv_t<OtherValueType>>::value>>
  constexpr bool
  operator!=(const strided_span<OtherValueType, OtherRank> &other) const {
    return !(*this == other);
  }

  template <typename OtherValueType, std::ptrdiff_t OtherRank,
            typename = std::enable_if_t<
                std::is_same<std::remove_cv_t<value_type>,
                             std::remove_cv_t<OtherValueType>>::value>>
  constexpr bool
  operator<(const strided_span<OtherValueType, OtherRank> &other) const {
    return std::lexicographical_compare(this->begin(), this->end(),
                                        other.begin(), other.end());
  }

  template <typename OtherValueType, std::ptrdiff_t OtherRank,
            typename = std::enable_if_t<
                std::is_same<std::remove_cv_t<value_type>,
                             std::remove_cv_t<OtherValueType>>::value>>
  constexpr bool
  operator<=(const strided_span<OtherValueType, OtherRank> &other) const {
    return !(other < *this);
  }

  template <typename OtherValueType, std::ptrdiff_t OtherRank,
            typename = std::enable_if_t<
                std::is_same<std::remove_cv_t<value_type>,
                             std::remove_cv_t<OtherValueType>>::value>>
  constexpr bool
  operator>(const strided_span<OtherValueType, OtherRank> &other) const {
    return (other < *this);
  }

  template <typename OtherValueType, std::ptrdiff_t OtherRank,
            typename = std::enable_if_t<
                std::is_same<std::remove_cv_t<value_type>,
                             std::remove_cv_t<OtherValueType>>::value>>
  constexpr bool
  operator>=(const strided_span<OtherValueType, OtherRank> &other) const {
    return !(*this < other);
  }

private:
  static index_type resize_extent(const index_type &extent, std::ptrdiff_t d) {
    // The last dimension of the array needs to contain a multiple of new type
    // elements
    GSL_SUPPRESS(bounds .4) // NO-FORMAT: attribute
    Expects(extent[Rank - 1] >= d && (extent[Rank - 1] % d == 0));

    index_type ret = extent;
    ret[Rank - 1] /= d;

    return ret;
  }

  template <bool Enabled = (Rank == 1), typename = std::enable_if_t<Enabled>>
  static index_type resize_stride(const index_type &strides, std::ptrdiff_t,
                                  void * = nullptr) {
    // Only strided arrays with regular strides can be resized
    GSL_SUPPRESS(bounds .4) // NO-FORMAT: attribute
    Expects(strides[Rank - 1] == 1);

    return strides;
  }

  template <bool Enabled = (Rank > 1), typename = std::enable_if_t<Enabled>>
  GSL_SUPPRESS(bounds .4) // NO-FORMAT: attribute
  static index_type resize_stride(const index_type &strides, std::ptrdiff_t d) {
    // Only strided arrays with regular strides can be resized
    Expects(strides[Rank - 1] == 1);
    // The strides must have contiguous chunks of
    // memory that can contain a multiple of new type elements
    Expects(strides[Rank - 2] >= d && (strides[Rank - 2] % d == 0));

    for (std::size_t i = Rank - 1; i > 0; --i) {
      // Only strided arrays with regular strides can be resized
      Expects((strides[i - 1] >= strides[i]) &&
              (strides[i - 1] % strides[i] == 0));
    }

    index_type ret = strides / d;
    ret[Rank - 1] = 1;

    return ret;
  }
};

template <class Span> class contiguous_span_iterator {
public:
  using iterator_category = std::random_access_iterator_tag;
  using value_type = typename Span::value_type;
  using difference_type = std::ptrdiff_t;
  using pointer = value_type *;
  using reference = value_type &;

private:
  template <typename ValueType, std::ptrdiff_t FirstDimension,
            std::ptrdiff_t... RestDimensions>
  friend class multi_span;

  pointer data_;
  const Span *m_validator;

  GSL_SUPPRESS(bounds .1) // NO-FORMAT: attribute
  void validateThis() const {
    // iterator is out of range of the array
    Expects(data_ >= m_validator->data_ &&
            data_ < m_validator->data_ + m_validator->size());
  }

  GSL_SUPPRESS(bounds .1) // NO-FORMAT: attribute
  contiguous_span_iterator(const Span *container, bool isbegin)
      : data_(isbegin ? container->data_
                      : container->data_ + container->size()),
        m_validator(container) {}

public:
  reference operator*() const {
    validateThis();
    return *data_;
  }
  pointer operator->() const {
    validateThis();
    return data_;
  }

  GSL_SUPPRESS(bounds .1) // NO-FORMAT: attribute
  contiguous_span_iterator &operator++() noexcept {
    ++data_;
    return *this;
  }
  contiguous_span_iterator operator++(int)noexcept {
    auto ret = *this;
    ++(*this);
    return ret;
  }

  GSL_SUPPRESS(bounds .1) // NO-FORMAT: attribute
  contiguous_span_iterator &operator--() noexcept {
    --data_;
    return *this;
  }
  contiguous_span_iterator operator--(int)noexcept {
    auto ret = *this;
    --(*this);
    return ret;
  }
  contiguous_span_iterator operator+(difference_type n) const noexcept {
    contiguous_span_iterator ret{*this};
    return ret += n;
  }
  contiguous_span_iterator &operator+=(difference_type n) noexcept {
    data_ += n;
    return *this;
  }
  contiguous_span_iterator operator-(difference_type n) const noexcept {
    contiguous_span_iterator ret{*this};
    return ret -= n;
  }

  contiguous_span_iterator &operator-=(difference_type n) {
    return *this += -n;
  }
  difference_type operator-(const contiguous_span_iterator &rhs) const {
    Expects(m_validator == rhs.m_validator);
    return data_ - rhs.data_;
  }
  reference operator[](difference_type n) const { return *(*this + n); }
  bool operator==(const contiguous_span_iterator &rhs) const {
    Expects(m_validator == rhs.m_validator);
    return data_ == rhs.data_;
  }

  bool operator!=(const contiguous_span_iterator &rhs) const {
    return !(*this == rhs);
  }

  bool operator<(const contiguous_span_iterator &rhs) const {
    Expects(m_validator == rhs.m_validator);
    return data_ < rhs.data_;
  }

  bool operator<=(const contiguous_span_iterator &rhs) const {
    return !(rhs < *this);
  }
  bool operator>(const contiguous_span_iterator &rhs) const {
    return rhs < *this;
  }
  bool operator>=(const contiguous_span_iterator &rhs) const {
    return !(rhs > *this);
  }

  void swap(contiguous_span_iterator &rhs) noexcept {
    std::swap(data_, rhs.data_);
    std::swap(m_validator, rhs.m_validator);
  }
};

template <typename Span>
contiguous_span_iterator<Span>
operator+(typename contiguous_span_iterator<Span>::difference_type n,
          const contiguous_span_iterator<Span> &rhs) noexcept {
  return rhs + n;
}

template <typename Span> class general_span_iterator {
public:
  using iterator_category = std::random_access_iterator_tag;
  using value_type = typename Span::value_type;
  using difference_type = std::ptrdiff_t;
  using pointer = value_type *;
  using reference = value_type &;

private:
  template <typename ValueType, std::size_t Rank> friend class strided_span;

  const Span *m_container;
  typename Span::bounds_type::iterator m_itr;
  general_span_iterator(const Span *container, bool isbegin)
      : m_container(container), m_itr(isbegin ? m_container->bounds().begin()
                                              : m_container->bounds().end()) {}

public:
  reference operator*() noexcept { return (*m_container)[*m_itr]; }
  pointer operator->() noexcept { return &(*m_container)[*m_itr]; }
  general_span_iterator &operator++() noexcept {
    ++m_itr;
    return *this;
  }
  general_span_iterator operator++(int)noexcept {
    auto ret = *this;
    ++(*this);
    return ret;
  }
  general_span_iterator &operator--() noexcept {
    --m_itr;
    return *this;
  }
  general_span_iterator operator--(int)noexcept {
    auto ret = *this;
    --(*this);
    return ret;
  }
  general_span_iterator operator+(difference_type n) const noexcept {
    general_span_iterator ret{*this};
    return ret += n;
  }
  general_span_iterator &operator+=(difference_type n) noexcept {
    m_itr += n;
    return *this;
  }
  general_span_iterator operator-(difference_type n) const noexcept {
    general_span_iterator ret{*this};
    return ret -= n;
  }
  general_span_iterator &operator-=(difference_type n) noexcept {
    return *this += -n;
  }
  difference_type operator-(const general_span_iterator &rhs) const {
    Expects(m_container == rhs.m_container);
    return m_itr - rhs.m_itr;
  }

  GSL_SUPPRESS(bounds .4) // NO-FORMAT: attribute
  value_type operator[](difference_type n) const {
    return (*m_container)[m_itr[n]];
  }

  bool operator==(const general_span_iterator &rhs) const {
    Expects(m_container == rhs.m_container);
    return m_itr == rhs.m_itr;
  }
  bool operator!=(const general_span_iterator &rhs) const {
    return !(*this == rhs);
  }
  bool operator<(const general_span_iterator &rhs) const {
    Expects(m_container == rhs.m_container);
    return m_itr < rhs.m_itr;
  }
  bool operator<=(const general_span_iterator &rhs) const {
    return !(rhs < *this);
  }
  bool operator>(const general_span_iterator &rhs) const { return rhs < *this; }
  bool operator>=(const general_span_iterator &rhs) const {
    return !(rhs > *this);
  }
  void swap(general_span_iterator &rhs) noexcept {
    std::swap(m_itr, rhs.m_itr);
    std::swap(m_container, rhs.m_container);
  }
};

template <typename Span>
general_span_iterator<Span>
operator+(typename general_span_iterator<Span>::difference_type n,
          const general_span_iterator<Span> &rhs) noexcept {
  return rhs + n;
}

} // namespace gsl

#if defined(_MSC_VER) && !defined(__clang__)
#if _MSC_VER < 1910

#undef constexpr
#pragma pop_macro("constexpr")
#endif // _MSC_VER < 1910

#pragma warning(pop)

#endif // _MSC_VER

#if defined(__GNUC__) && __GNUC__ > 6
#pragma GCC diagnostic pop
#endif // __GNUC__ > 6

#endif // GSL_MULTI_SPAN_H
//------------------------------------------------------------------------------
// END multi_span
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
// Start turntable.h
//------------------------------------------------------------------------------
#include <cmath>
#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

namespace givr {
namespace camera {
constexpr const float LONGITUDE_MAX = 2. * M_PI;
constexpr const float LATITUDE_MAX = M_PI;

struct TurnTableCamera : public Camera<Longitude, Latitude, Zoom, Translation> {
  // TODO: ensure these are in the right ranges.
  TurnTableCamera() {
    reset();
  }

  void reset()
  {
    set(Latitude(LATITUDE_MAX / 2.));
    set(Longitude(0.f));
    set(Zoom(75.f));
    set(Translation(0.f, 0.f, 0.f));
  }

  float const &latitude() const { return value<Latitude>().value(); }
  float const &longitude() const { return value<Longitude>().value(); }
  float const &zoom() const { return value<Zoom>().value(); }
  vec3f const &translation() const { return value<Translation>().value(); }

  float &latitude() { return value<Latitude>().value(); }
  float &longitude() { return value<Longitude>().value(); }
  float &zoom() { return value<Zoom>().value(); }
  vec3f &translation() { return value<Translation>().value(); }

  void rotateAroundXPercent(float perc);
  void rotateAroundYPercent(float perc);
  void rotateAroundX(float angle);
  void rotateAroundY(float angle);
  void zoom(float amount);
  void translate(vec3f amount);

  mat4f viewMatrix() const;
  vec3f viewPosition() const;
};

template <typename... Args> TurnTableCamera TurnTable(Args &&... args) {
  using namespace utility;
  static_assert(!has_duplicate_types<Args...>,
                "The arguments you passed in have duplicate parameters");

  static_assert(is_subset_of<std::tuple<Args...>, TurnTableCamera::Args> &&
                    sizeof...(args) <=
                        std::tuple_size<TurnTableCamera::Args>::value,
                "You have provided incorrect parameters for TurnTable. "
                "Longitude, Latitude, Zoom and Translation are optional.");
  TurnTableCamera pv;

  if constexpr (sizeof...(args) > 0) {
    pv.set(std::forward<Args>(args)...);
  }
  return pv;
}

} // end namespace camera
} // end namespace givr
//------------------------------------------------------------------------------
// END turntable.h
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
// Start perspective.h
//------------------------------------------------------------------------------

namespace givr {
namespace camera {

struct PerspectiveProjection
    : public Projection<FieldOfViewY, AspectRatio, NearDistance, FarDistance> {
  PerspectiveProjection() {
    reset();
  }

  void reset()
  {
    set(FieldOfViewY(45.f));
    set(AspectRatio(4.f / 3.f));
    set(NearDistance(0.1f));
    set(FarDistance(1000.f));
  }

  float const &fieldOfViewY() const { return value<FieldOfViewY>().value(); }
  float const &aspectRatio() const { return value<AspectRatio>().value(); }
  float const &nearDistance() const { return value<NearDistance>().value(); }
  float const &farDistance() const { return value<FarDistance>().value(); }

  float &fieldOfViewY() { return value<FieldOfViewY>().value(); }
  float &aspectRatio() { return value<AspectRatio>().value(); }
  float &nearDistance() { return value<NearDistance>().value(); }
  float &farDistance() { return value<FarDistance>().value(); }
  void updateAspectRatio(int width, int height);

  mat4f projectionMatrix() const;
};

template <typename... Args> PerspectiveProjection Perspective(Args &&... args) {
  using namespace utility;
  static_assert(!has_duplicate_types<Args...>,
                "The arguments you passed in have duplicate parameters");

  static_assert(
      is_subset_of<std::tuple<Args...>, PerspectiveProjection::Args> &&
          sizeof...(args) <=
              std::tuple_size<PerspectiveProjection::Args>::value,
      "You have provided incorrect parameters for Perspective. "
      "FieldOfViewY, AspectRatio, NearDistance, FarDistance are optional.");
  PerspectiveProjection pv;

  if constexpr (sizeof...(args) > 0) {
    pv.set(std::forward<Args>(args)...);
  }
  return pv;
}

} // end namespace camera
} // end namespace givr
//------------------------------------------------------------------------------
// END perspective.h
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
// Start orthographic.h
//------------------------------------------------------------------------------

namespace givr {
namespace camera {

struct OrthographicProjection
    : public Projection<Left, Right, Bottom, Top, NearDistance, FarDistance,
                        AspectRatio> {
  OrthographicProjection() {
    set(Left(-1.0));
    set(Right(1.0));
    set(Bottom(-1.0));
    set(Top(1.0));
    set(NearDistance(0.1f));
    set(FarDistance(1000.f));
    set(AspectRatio(1.0f));
  }

  float const &left() const { return value<Left>().value(); }
  float const &right() const { return value<Right>().value(); }
  float const &top() const { return value<Top>().value(); }
  float const &bottom() const { return value<Bottom>().value(); }
  float const &aspectRatio() const { return value<AspectRatio>().value(); }
  float const &nearDistance() const { return value<NearDistance>().value(); }
  float const &farDistance() const { return value<FarDistance>().value(); }

  float &left() { return value<Left>().value(); }
  float &right() { return value<Right>().value(); }
  float &top() { return value<Top>().value(); }
  float &bottom() { return value<Bottom>().value(); }
  float &aspectRatio() { return value<AspectRatio>().value(); }
  float &nearDistance() { return value<NearDistance>().value(); }
  float &farDistance() { return value<FarDistance>().value(); }

  void updateAspectRatio(int width, int height);
  mat4f projectionMatrix() const;
};

template <typename... Args>
OrthographicProjection Orthographic(Args &&... args) {
  using namespace utility;
  static_assert(!has_duplicate_types<Args...>,
                "The arguments you passed in have duplicate parameters");

  static_assert(
      is_subset_of<std::tuple<Args...>, OrthographicProjection::Args> &&
          sizeof...(args) <=
              std::tuple_size<OrthographicProjection::Args>::value,
      "You have provided incorrect parameters for Orthographic. "
      "Left, AspectRatio, NearDistance, FarDistance are optional.");
  OrthographicProjection pv;

  if constexpr (sizeof...(args) > 0) {
    pv.set(std::forward<Args>(args)...);
  }
  return pv;
}

} // namespace camera
} // end namespace givr
//------------------------------------------------------------------------------
// END orthographic.h
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
// Start triangle.h
//------------------------------------------------------------------------------
#include <vector>

namespace givr {
namespace geometry {

struct Triangle : public Geometry<Point1, Point2, Point3> {
  template <typename... Args> Triangle(Args &&... args) {
    using required_args = std::tuple<Point1, Point2, Point3>;

    using namespace utility;
    static_assert(!has_duplicate_types<Args...>,
                  "The arguments you passed in have duplicate parameters");

    static_assert(is_subset_of<required_args, std::tuple<Args...>> &&
                      is_subset_of<std::tuple<Args...>, Triangle::Args> &&
                      sizeof...(args) <= std::tuple_size<Triangle::Args>::value,
                  "You have provided incorrect parameters for Triangle. "
                  "Point1, Point2 and Point3 are required.");
    set(std::forward<Args>(args)...);
  }
  vec3f const &p1() const { return value<Point1>().value(); }
  vec3f const &p2() const { return value<Point2>().value(); }
  vec3f const &p3() const { return value<Point3>().value(); }

  vec3f &p1() { return value<Point1>().value(); }
  vec3f &p2() { return value<Point2>().value(); }
  vec3f &p3() { return value<Point3>().value(); }

  struct Data : public VertexArrayData<PrimitiveType::TRIANGLES> {
    std::uint16_t dimensions = 3;
    BufferUsageType verticesType = BufferUsageType::STATIC_DRAW;
    std::vector<float> vertices;

    BufferUsageType normalsType = BufferUsageType::STATIC_DRAW;
    std::vector<float> normals;
  };
};
// Backwards Compatibility
using TriangleGeometry = Triangle;

Triangle::Data generateGeometry(Triangle const &t);
} // end namespace geometry
} // end namespace givr
//------------------------------------------------------------------------------
// END triangle.h
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
// Start custom.h
//------------------------------------------------------------------------------
#include <cstddef>
#include <cstdint>
#include <utility>
#include <vector>

namespace givr {
namespace geometry {

template <PrimitiveType PrimitiveT> struct CustomGeometry {
  std::vector<vec3f> vertices;
  std::vector<vec3f> normals;
  std::vector<std::uint32_t> indices;
  std::vector<vec3f> colours;
  std::vector<vec2f> uvs;

  // TODO: add simpler ways to construct this.

  struct Data : public VertexArrayData<PrimitiveT> {
    std::uint16_t dimensions = 3;

    BufferUsageType verticesType;
    BufferUsageType normalsType;
    BufferUsageType indicesType;
    BufferUsageType coloursType;
    BufferUsageType uvsType;

    gsl::span<const float> vertices;
    gsl::span<const float> normals;
    gsl::span<const uint32_t> indices;
    gsl::span<const float> colours;
    gsl::span<const float> uvs;
  };
};

template <typename VectorT, int vecsize>
void __customGeometryCopy(std::vector<VectorT> const &source,
                          std::vector<float> &target) {
  for (std::size_t i = 0; i < source.size(); ++i) {
    for (std::size_t j = 0; j < vecsize; ++j) {
      target.push_back(source[i][j]);
    }
  }
};

template <PrimitiveType PrimitiveT>
typename CustomGeometry<PrimitiveT>::Data
generateGeometry(CustomGeometry<PrimitiveT> const &l) {
  typename CustomGeometry<PrimitiveT>::Data data;
  data.vertices =
      gsl::span<const float>(reinterpret_cast<float const *>(l.vertices.data()),
                             l.vertices.size() * 3);
  data.normals = gsl::span<const float>(
      reinterpret_cast<float const *>(l.normals.data()), l.normals.size() * 3);
  data.colours = gsl::span<const float>(
      reinterpret_cast<float const *>(l.colours.data()), l.colours.size() * 3);
  data.uvs = gsl::span<const float>(
      reinterpret_cast<float const *>(l.uvs.data()), l.uvs.size() * 2);
  data.indices = gsl::span<const std::uint32_t>(l.indices);

  return data;
}

} // end namespace geometry
} // end namespace givr
//------------------------------------------------------------------------------
// END custom.h
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
// Start polyline.h
//------------------------------------------------------------------------------
#include <vector>

namespace givr {
namespace geometry {

template <PrimitiveType LineType> struct PolyLine {
  static_assert(LineType != PrimitiveType::LINE_LOOP ||
                    LineType != PrimitiveType::LINE_STRIP,
                "PolyLine only supports LINE_LOOP or LINE_STRIP");

private:
  std::vector<Point> m_points;

public:
  template <typename... Args> PolyLine(Args &&... args) {
    points() = {args...};
  }
  std::vector<Point> &points() { return m_points; }
  std::vector<Point> const &points() const { return m_points; }
  std::vector<Point> &operator*() { return m_points; }
  // TODO: expose more of the vector interface here, or
  //       the conversion operator or something
  void push_back(Point const &p) { m_points.push_back(p); }
  void clear() { m_points.clear(); }

  struct Data : public VertexArrayData<LineType> {
    std::uint16_t dimensions = 3;

    BufferUsageType verticesType;
    std::vector<float> vertices;
  };
};
// Backwards Compatibility
template <PrimitiveType LineType> using PolyLineGeometry = PolyLine<LineType>;

template <PrimitiveType LineType>
typename PolyLine<LineType>::Data
generateGeometry(PolyLine<LineType> const &l) {
  typename PolyLine<LineType>::Data data;
  auto const &points = l.points();
  data.vertices.reserve(points.size() * 3);
  for (std::size_t i = 0; i < points.size(); ++i) {
    data.vertices.push_back(points[i].value()[0]);
    data.vertices.push_back(points[i].value()[1]);
    data.vertices.push_back(points[i].value()[2]);
  }
  return data;
}
} // end namespace geometry
} // end namespace givr
//------------------------------------------------------------------------------
// END polyline.h
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
// Start quad.h
//------------------------------------------------------------------------------
#include <vector>

namespace givr {
namespace geometry {

struct Quad : public Geometry<Point1, Point2, Point3, Point4> {
  template <typename... Args> Quad(Args &&... args) {
    using required_args = std::tuple<Point1, Point2, Point3>;

    using namespace utility;
    static_assert(!has_duplicate_types<Args...>,
                  "The arguments you passed in have duplicate parameters");
    static_assert(
        is_subset_of<required_args, std::tuple<Args...>> &&
            is_subset_of<std::tuple<Args...>, Quad::Args> &&
            sizeof...(args) <= std::tuple_size<Quad::Args>::value,
        "You have provided incorrect parameters for Quad. "
        "Point1, Point2, and Point3 are required. Point4 is optional. "
        "If you provide Point4, then you are responsible for ensuring it "
        "is co-planar with Point1, Point2, and Point3");
    if constexpr (sizeof...(args) > 0) {
      set(std::forward<Args>(args)...);
    }
    if constexpr (sizeof...(args) == 3) {
      set(Point4(p2() + (p3() - p1())));
    }
  }

  vec3f const &p1() const { return value<Point1>().value(); }
  vec3f const &p2() const { return value<Point2>().value(); }
  vec3f const &p3() const { return value<Point3>().value(); }
  vec3f const &p4() const { return value<Point4>().value(); }

  vec3f &p1() { return value<Point1>().value(); }
  vec3f &p2() { return value<Point2>().value(); }
  vec3f &p3() { return value<Point3>().value(); }
  vec3f &p4() { return value<Point4>().value(); }

  struct Data : public VertexArrayData<PrimitiveType::TRIANGLES> {
    std::uint16_t dimensions = 3;

    BufferUsageType verticesType = BufferUsageType::STATIC_DRAW;
    std::vector<float> vertices;

    BufferUsageType normalsType = BufferUsageType::STATIC_DRAW;
    std::vector<float> normals;

    BufferUsageType indicesType = BufferUsageType::STATIC_DRAW;
    std::vector<std::uint32_t> indices;

    BufferUsageType uvsType = BufferUsageType::STATIC_DRAW;
    std::vector<float> uvs;
  };
};
// Backwards compatibility
using QuadGeometry = Quad;

Quad::Data generateGeometry(Quad const &t);
} // end namespace geometry
} // end namespace givr
//------------------------------------------------------------------------------
// END quad.h
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
// Start mesh.h
//------------------------------------------------------------------------------

#include <cstddef>
#include <cstdint>
#include <vector>

namespace givr {
namespace geometry {
struct Mesh : public Geometry<Filename>
// TODO: Add other parameters like smooth shading etc.
{
  template <typename... Args> Mesh(Args &&... args) {
    using required_args = std::tuple<Filename>;

    using namespace utility;
    static_assert(!has_duplicate_types<Args...>,
                  "The arguments you passed in have duplicate parameters");

    static_assert(is_subset_of<required_args, std::tuple<Args...>>,
                  "Filename is a required parameter for Mesh. "
                  "Please provide them.");
    static_assert(is_subset_of<std::tuple<Args...>, Mesh::Args>,
                  "You have provided incorrect parameters for Mesh. "
                  "Filename is required.");
    static_assert(sizeof...(args) <= std::tuple_size<Mesh::Args>::value,
                  "You have provided incorrect parameters for Mesh. "
                  "Filename is required.");
    set(std::forward<Args>(args)...);
  }

  std::string const &filename() const { return value<Filename>().value(); }
  std::string &filename() { return value<Filename>().value(); }

  struct Data : VertexArrayData<PrimitiveType::TRIANGLES> {
    std::uint16_t dimensions = 3;

    BufferUsageType verticesType = BufferUsageType::STATIC_DRAW;
    BufferUsageType normalsType = BufferUsageType::STATIC_DRAW;
    BufferUsageType indicesType = BufferUsageType::STATIC_DRAW;
    BufferUsageType uvsType = BufferUsageType::STATIC_DRAW;

    std::vector<float> vertices;
    std::vector<float> normals;
    std::vector<std::uint32_t> indices;
    std::vector<float> uvs;
  };
};
// Backwards compatibility
using MeshGeometry = Mesh;

Mesh::Data generateGeometry(const Mesh &m);

} // end namespace geometry
} // end namespace givr
//------------------------------------------------------------------------------
// END mesh.h
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
// Start line.h
//------------------------------------------------------------------------------
#include <vector>

namespace givr {
namespace geometry {

struct Line : public Geometry<Point1, Point2> {
  template <typename... Args> Line(Args &&... args) {
    using required_args = std::tuple<Point1, Point2>;

    using namespace utility;
    static_assert(!has_duplicate_types<Args...>,
                  "The arguments you passed in have duplicate parameters");

    static_assert(is_subset_of<required_args, std::tuple<Args...>>,
                  "Point1 and Point2 are required parameters for Line. "
                  "Please provide them.");
    static_assert(is_subset_of<std::tuple<Args...>, Line::Args>,
                  "You have provided incorrect parameters for Line. "
                  "Point1 and Point2 are required.");
    static_assert(sizeof...(args) <= std::tuple_size<Line::Args>::value,
                  "You have provided incorrect parameters for Line. "
                  "Point1 and Point2 are required.");
    set(std::forward<Args>(args)...);
  }

  vec3f const &p1() const { return value<Point1>().value(); }
  vec3f const &p2() const { return value<Point2>().value(); }

  vec3f &p1() { return value<Point1>().value(); }
  vec3f &p2() { return value<Point2>().value(); }

  struct Data : public VertexArrayData<PrimitiveType::LINES> {
    std::uint16_t dimensions = 3;

    BufferUsageType verticesType = BufferUsageType::STATIC_DRAW;
    std::vector<float> vertices;
  };
};
// Backwards Compatibility
using LineGeometry = Line;

Line::Data generateGeometry(Line const &l);
} // end namespace geometry
} // end namespace givr
//------------------------------------------------------------------------------
// END line.h
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
// Start cylinder.h
//------------------------------------------------------------------------------
#include <vector>

namespace givr {
namespace geometry {

struct Cylinder : public Geometry<Point1, Point2, Radius, AzimuthPoints> {
  template <typename... Args> Cylinder(Args &&... args) {
    using required_args = std::tuple<Point1, Point2>;

    using namespace utility;
    static_assert(!has_duplicate_types<Args...>,
                  "The arguments you passed in have duplicate parameters");

    static_assert(is_subset_of<required_args, std::tuple<Args...>>,
                  "Point1 and Point2 are a required parameter for Cylinder. "
                  "Please provide them.");
    static_assert(is_subset_of<std::tuple<Args...>, Cylinder::Args>,
                  "You have provided incorrect parameters for Cylinder. "
                  "Point1 and Point2 are required. Radius and AzimuthPoints "
                  "are optional.");
    static_assert(sizeof...(args) <= std::tuple_size<Cylinder::Args>::value,
                  "You have provided incorrect parameters for Cylinder. "
                  "Point1 and Point2 are required. Radius and AzimuthPoints "
                  "are optional.");
    set(Point1(0.f, 0.5f, 0.f));
    set(Point2(0.f, -0.5f, 0.f));
    set(Radius(1.0f));
    set(AzimuthPoints(20));
    if constexpr (sizeof...(args) > 0) {
      set(std::forward<Args>(args)...);
    }
  }

  vec3f const &p1() const { return value<Point1>().value(); }
  vec3f const &p2() const { return value<Point2>().value(); }

  vec3f &p1() { return value<Point1>().value(); }
  vec3f &p2() { return value<Point2>().value(); }

  float radius() const { return value<Radius>().value(); }
  std::size_t azimuthPoints() const { return value<AzimuthPoints>().value(); }

  struct Data : public VertexArrayData<PrimitiveType::TRIANGLES> {
    std::uint16_t dimensions = 3;

    BufferUsageType verticesType = BufferUsageType::STATIC_DRAW;
    std::vector<float> vertices;

    BufferUsageType normalsType = BufferUsageType::STATIC_DRAW;
    std::vector<float> normals;

    BufferUsageType indicesType = BufferUsageType::STATIC_DRAW;
    std::vector<std::uint32_t> indices;
  };
};

// Backwards Compatibility
using CylinderGeometry = Cylinder;

Cylinder::Data generateGeometry(Cylinder const &l);
} // end namespace geometry
} // end namespace givr
//------------------------------------------------------------------------------
// END cylinder.h
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
// Start sphere.h
//------------------------------------------------------------------------------
#include <cstddef>
#include <cstdint>
#include <vector>

namespace givr {
namespace geometry {

struct Sphere
    : public Geometry<Centroid, Radius, AzimuthPoints, AltitudePoints> {
  template <typename... Args> Sphere(Args &&... args) {
    using namespace utility;
    static_assert(!has_duplicate_types<Args...>,
                  "The arguments you passed in have duplicate parameters");

    static_assert(
        is_subset_of<std::tuple<Args...>, Sphere::Args>,
        "You have provided incorrect parameters for Sphere. "
        "Centroid, Radius, AzimuthPoints, AltitudePoints are optional.");
    static_assert(
        sizeof...(args) <= std::tuple_size<Sphere::Args>::value,
        "You have provided incorrect parameters for Sphere. "
        "Centroid, Radius, AzimuthPoints, AltitudePoints are optional.");
    this->set(Centroid(0.0, 0.0, 0.0));
    this->set(Radius(1.0));
    this->set(AzimuthPoints(20.));
    this->set(AltitudePoints(20.));
    if constexpr (sizeof...(args) > 0) {
      this->set(std::forward<Args>(args)...);
    }
  }

  struct Data : public VertexArrayData<PrimitiveType::TRIANGLES> {
    std::uint16_t dimensions = 3;

    BufferUsageType verticesType = BufferUsageType::STATIC_DRAW;
    BufferUsageType normalsType = BufferUsageType::STATIC_DRAW;
    BufferUsageType indicesType = BufferUsageType::STATIC_DRAW;
    BufferUsageType uvsType = BufferUsageType::STATIC_DRAW;

    std::vector<float> vertices;
    std::vector<float> normals;
    std::vector<std::uint32_t> indices;
    std::vector<float> uvs;
  };
};
// Backwards Compatibility
using SphereGeometry = Sphere;

Sphere::Data generateGeometry(Sphere const &s);

} // end namespace geometry
} // end namespace givr
//------------------------------------------------------------------------------
// END sphere.h
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
// Start buffer.h
//------------------------------------------------------------------------------

#include <array>
#include <utility>
#include <vector>

namespace givr {

class Buffer {
public:
  Buffer();
  // TODO(lw): make a version that just receives the source directly.

  // Default ctor/dtor & move operations
  Buffer(Buffer &&other) = default;
  Buffer &operator=(Buffer &&rhs) = default;

  // But no copy or assignment. Bad.
  Buffer(const Buffer &) = delete;
  Buffer &operator=(const Buffer &) = delete;

  ~Buffer();

  operator GLuint() const { return m_bufferID; }
  void alloc();
  void dealloc();
  void bind(GLenum target);
  void unbind(GLenum target);
  template <typename T>
  void data(GLenum target, const gsl::span<T> &data, GLenum usage) {
    glBufferData(target, sizeof(T) * data.size(), data.data(), usage);
  }
  template <typename T>
  void data(GLenum target, const std::vector<T> &data, GLenum usage) {
    glBufferData(target, sizeof(T) * data.size(), data.data(), usage);
  }

private:
  GLuint m_bufferID = 0;
};
}; // end namespace givr
//------------------------------------------------------------------------------
// END buffer.h
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
// Start gsl_algorithm
//------------------------------------------------------------------------------
///////////////////////////////////////////////////////////////////////////////
//
// Copyright (c) 2015 Microsoft Corporation. All rights reserved.
//
// This code is licensed under the MIT License (MIT).
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
// THE SOFTWARE.
//
///////////////////////////////////////////////////////////////////////////////

#ifndef GSL_ALGORITHM_H
#define GSL_ALGORITHM_H

#include <algorithm>   // for copy_n
#include <cstddef>     // for ptrdiff_t
#include <type_traits> // for is_assignable

#ifdef _MSC_VER
#pragma warning(push)

// turn off some warnings that are noisy about our Expects statements
#pragma warning(disable : 4127) // conditional expression is constant
#pragma warning(disable : 4996) // unsafe use of std::copy_n

#endif // _MSC_VER

namespace gsl {
// Note: this will generate faster code than std::copy using span iterator in
// older msvc+stl not necessary for msvc since VS2017 15.8 (_MSC_VER >= 1915)
template <class SrcElementType, std::ptrdiff_t SrcExtent, class DestElementType,
          std::ptrdiff_t DestExtent>
void copy(span<SrcElementType, SrcExtent> src,
          span<DestElementType, DestExtent> dest) {
  static_assert(
      std::is_assignable<decltype(*dest.data()), decltype(*src.data())>::value,
      "Elements of source span can not be assigned to elements of destination "
      "span");
  static_assert(SrcExtent == dynamic_extent || DestExtent == dynamic_extent ||
                    (SrcExtent <= DestExtent),
                "Source range is longer than target range");

  Expects(dest.size() >= src.size());
  GSL_SUPPRESS(stl .1) // NO-FORMAT: attribute
  std::copy_n(src.data(), src.size(), dest.data());
}

} // namespace gsl

#ifdef _MSC_VER
#pragma warning(pop)
#endif // _MSC_VER

#endif // GSL_ALGORITHM_H
//------------------------------------------------------------------------------
// END gsl_algorithm
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
// Start string_span
//------------------------------------------------------------------------------
///////////////////////////////////////////////////////////////////////////////
//
// Copyright (c) 2015 Microsoft Corporation. All rights reserved.
//
// This code is licensed under the MIT License (MIT).
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
// THE SOFTWARE.
//
///////////////////////////////////////////////////////////////////////////////

#ifndef GSL_STRING_SPAN_H
#define GSL_STRING_SPAN_H

#include <algorithm> // for equal, lexicographical_compare
#include <array>     // for array
#include <cstddef>   // for ptrdiff_t, size_t, nullptr_t
#include <cstdint>   // for PTRDIFF_MAX
#include <cstring>
#include <string>      // for basic_string, allocator, char_traits
#include <type_traits> // for declval, is_convertible, enable_if_t, add_...

#if defined(_MSC_VER) && !defined(__clang__)
#pragma warning(push)

// Turn MSVC /analyze rules that generate too much noise. TODO: fix in the tool.
#pragma warning(                                                               \
    disable : 26446) // TODO: bug in parser - attributes and templates
#pragma warning(disable : 26481) // TODO: suppress does not work inside
                                 // templates sometimes

#if _MSC_VER < 1910
#pragma push_macro("constexpr")
#define constexpr /*constexpr*/

#endif // _MSC_VER < 1910
#endif // _MSC_VER

namespace gsl {
//
// czstring and wzstring
//
// These are "tag" typedefs for C-style strings (i.e. null-terminated character
// arrays) that allow static analysis to help find bugs.
//
// There are no additional features/semantics that we can find a way to add
// inside the type system for these types that will not either incur significant
// runtime costs or (sometimes needlessly) break existing programs when
// introduced.
//

template <typename CharT, std::ptrdiff_t Extent = dynamic_extent>
using basic_zstring = CharT *;

template <std::ptrdiff_t Extent = dynamic_extent>
using czstring = basic_zstring<const char, Extent>;

template <std::ptrdiff_t Extent = dynamic_extent>
using cwzstring = basic_zstring<const wchar_t, Extent>;

template <std::ptrdiff_t Extent = dynamic_extent>
using cu16zstring = basic_zstring<const char16_t, Extent>;

template <std::ptrdiff_t Extent = dynamic_extent>
using cu32zstring = basic_zstring<const char32_t, Extent>;

template <std::ptrdiff_t Extent = dynamic_extent>
using zstring = basic_zstring<char, Extent>;

template <std::ptrdiff_t Extent = dynamic_extent>
using wzstring = basic_zstring<wchar_t, Extent>;

template <std::ptrdiff_t Extent = dynamic_extent>
using u16zstring = basic_zstring<char16_t, Extent>;

template <std::ptrdiff_t Extent = dynamic_extent>
using u32zstring = basic_zstring<char32_t, Extent>;

namespace details {
template <class CharT>
std::ptrdiff_t string_length(const CharT *str, std::ptrdiff_t n) {
  if (str == nullptr || n <= 0)
    return 0;

  const span<const CharT> str_span{str, n};

  std::ptrdiff_t len = 0;
  while (len < n && str_span[len])
    len++;

  return len;
}
} // namespace details

//
// ensure_sentinel()
//
// Provides a way to obtain an span from a contiguous sequence
// that ends with a (non-inclusive) sentinel value.
//
// Will fail-fast if sentinel cannot be found before max elements are examined.
//
template <typename T, const T Sentinel>
span<T, dynamic_extent> ensure_sentinel(T *seq,
                                        std::ptrdiff_t max = PTRDIFF_MAX) {
  Ensures(seq != nullptr);

  GSL_SUPPRESS(f .23) // NO-FORMAT: attribute // TODO: false positive // TODO:
                      // suppress does not work
  auto cur = seq;
  Ensures(cur != nullptr); // workaround for removing the warning

  GSL_SUPPRESS(
      bounds .1) // NO-FORMAT: attribute // TODO: suppress does not work
  while ((cur - seq) < max && *cur != Sentinel)
    ++cur;
  Ensures(*cur == Sentinel);
  return {seq, cur - seq};
}

//
// ensure_z - creates a span for a zero terminated strings.
// Will fail fast if a null-terminator cannot be found before
// the limit of size_type.
//
template <typename CharT>
span<CharT, dynamic_extent> ensure_z(CharT *const &sz,
                                     std::ptrdiff_t max = PTRDIFF_MAX) {
  return ensure_sentinel<CharT, CharT(0)>(sz, max);
}

template <typename CharT, std::size_t N>
span<CharT, dynamic_extent> ensure_z(CharT (&sz)[N]) {
  return ensure_z(&sz[0], narrow_cast<std::ptrdiff_t>(N));
}

template <class Cont>
span<typename std::remove_pointer<typename Cont::pointer>::type, dynamic_extent>
ensure_z(Cont &cont) {
  return ensure_z(cont.data(), narrow_cast<std::ptrdiff_t>(cont.size()));
}

template <typename CharT, std::ptrdiff_t> class basic_string_span;

namespace details {
template <typename T> struct is_basic_string_span_oracle : std::false_type {};

template <typename CharT, std::ptrdiff_t Extent>
struct is_basic_string_span_oracle<basic_string_span<CharT, Extent>>
    : std::true_type {};

template <typename T>
struct is_basic_string_span : is_basic_string_span_oracle<std::remove_cv_t<T>> {
};
} // namespace details

//
// string_span and relatives
//
template <typename CharT, std::ptrdiff_t Extent = dynamic_extent>
class basic_string_span {
public:
  using element_type = CharT;
  using pointer = std::add_pointer_t<element_type>;
  using reference = std::add_lvalue_reference_t<element_type>;
  using const_reference =
      std::add_lvalue_reference_t<std::add_const_t<element_type>>;
  using impl_type = span<element_type, Extent>;

  using index_type = typename impl_type::index_type;
  using iterator = typename impl_type::iterator;
  using const_iterator = typename impl_type::const_iterator;
  using reverse_iterator = typename impl_type::reverse_iterator;
  using const_reverse_iterator = typename impl_type::const_reverse_iterator;

  // default (empty)
  constexpr basic_string_span() noexcept = default;

  // copy
  constexpr basic_string_span(const basic_string_span &other) noexcept =
      default;

  // assign
  constexpr basic_string_span &
  operator=(const basic_string_span &other) noexcept = default;

  constexpr basic_string_span(pointer ptr, index_type length)
      : span_(ptr, length) {}
  constexpr basic_string_span(pointer firstElem, pointer lastElem)
      : span_(firstElem, lastElem) {}

  // From static arrays - if 0-terminated, remove 0 from the view
  // All other containers allow 0s within the length, so we do not remove them
  template <std::size_t N>
  constexpr basic_string_span(element_type (&arr)[N]) : span_(remove_z(arr)) {}

  template <std::size_t N,
            class ArrayElementType = std::remove_const_t<element_type>>
  constexpr basic_string_span(std::array<ArrayElementType, N> &arr) noexcept
      : span_(arr) {}

  template <std::size_t N,
            class ArrayElementType = std::remove_const_t<element_type>>
  constexpr basic_string_span(
      const std::array<ArrayElementType, N> &arr) noexcept
      : span_(arr) {}

  // Container signature should work for basic_string after C++17 version exists
  template <class Traits, class Allocator>
  // GSL_SUPPRESS(bounds.4) // NO-FORMAT: attribute // TODO: parser bug
  constexpr basic_string_span(
      std::basic_string<element_type, Traits, Allocator> &str)
      : span_(&str[0], narrow_cast<std::ptrdiff_t>(str.length())) {}

  template <class Traits, class Allocator>
  constexpr basic_string_span(
      const std::basic_string<element_type, Traits, Allocator> &str)
      : span_(&str[0], str.length()) {}

  // from containers. Containers must have a pointer type and data() function
  // signatures
  template <
      class Container,
      class = std::enable_if_t<
          !details::is_basic_string_span<Container>::value &&
          std::is_convertible<typename Container::pointer, pointer>::value &&
          std::is_convertible<
              typename Container::pointer,
              decltype(std::declval<Container>().data())>::value>>
  constexpr basic_string_span(Container &cont) : span_(cont) {}

  template <
      class Container,
      class = std::enable_if_t<
          !details::is_basic_string_span<Container>::value &&
          std::is_convertible<typename Container::pointer, pointer>::value &&
          std::is_convertible<
              typename Container::pointer,
              decltype(std::declval<Container>().data())>::value>>
  constexpr basic_string_span(const Container &cont) : span_(cont) {}

  // from string_span
  template <
      class OtherValueType, std::ptrdiff_t OtherExtent,
      class = std::enable_if_t<std::is_convertible<
          typename basic_string_span<OtherValueType, OtherExtent>::impl_type,
          impl_type>::value>>
  constexpr basic_string_span(
      basic_string_span<OtherValueType, OtherExtent> other)
      : span_(other.data(), other.length()) {}

  template <index_type Count>
  constexpr basic_string_span<element_type, Count> first() const {
    return {span_.template first<Count>()};
  }

  constexpr basic_string_span<element_type, dynamic_extent>
  first(index_type count) const {
    return {span_.first(count)};
  }

  template <index_type Count>
  constexpr basic_string_span<element_type, Count> last() const {
    return {span_.template last<Count>()};
  }

  constexpr basic_string_span<element_type, dynamic_extent>
  last(index_type count) const {
    return {span_.last(count)};
  }

  template <index_type Offset, index_type Count>
  constexpr basic_string_span<element_type, Count> subspan() const {
    return {span_.template subspan<Offset, Count>()};
  }

  constexpr basic_string_span<element_type, dynamic_extent>
  subspan(index_type offset, index_type count = dynamic_extent) const {
    return {span_.subspan(offset, count)};
  }

  constexpr reference operator[](index_type idx) const { return span_[idx]; }
  constexpr reference operator()(index_type idx) const { return span_[idx]; }

  constexpr pointer data() const { return span_.data(); }

  constexpr index_type length() const noexcept { return span_.size(); }
  constexpr index_type size() const noexcept { return span_.size(); }
  constexpr index_type size_bytes() const noexcept {
    return span_.size_bytes();
  }
  constexpr index_type length_bytes() const noexcept {
    return span_.length_bytes();
  }
  constexpr bool empty() const noexcept { return size() == 0; }

  constexpr iterator begin() const noexcept { return span_.begin(); }
  constexpr iterator end() const noexcept { return span_.end(); }

  constexpr const_iterator cbegin() const noexcept { return span_.cbegin(); }
  constexpr const_iterator cend() const noexcept { return span_.cend(); }

  constexpr reverse_iterator rbegin() const noexcept { return span_.rbegin(); }
  constexpr reverse_iterator rend() const noexcept { return span_.rend(); }

  constexpr const_reverse_iterator crbegin() const noexcept {
    return span_.crbegin();
  }
  constexpr const_reverse_iterator crend() const noexcept {
    return span_.crend();
  }

private:
  static impl_type remove_z(pointer const &sz, std::ptrdiff_t max) {
    return {sz, details::string_length(sz, max)};
  }

  template <std::size_t N> static impl_type remove_z(element_type (&sz)[N]) {
    return remove_z(&sz[0], narrow_cast<std::ptrdiff_t>(N));
  }

  impl_type span_;
};

template <std::ptrdiff_t Extent = dynamic_extent>
using string_span = basic_string_span<char, Extent>;

template <std::ptrdiff_t Extent = dynamic_extent>
using cstring_span = basic_string_span<const char, Extent>;

template <std::ptrdiff_t Extent = dynamic_extent>
using wstring_span = basic_string_span<wchar_t, Extent>;

template <std::ptrdiff_t Extent = dynamic_extent>
using cwstring_span = basic_string_span<const wchar_t, Extent>;

template <std::ptrdiff_t Extent = dynamic_extent>
using u16string_span = basic_string_span<char16_t, Extent>;

template <std::ptrdiff_t Extent = dynamic_extent>
using cu16string_span = basic_string_span<const char16_t, Extent>;

template <std::ptrdiff_t Extent = dynamic_extent>
using u32string_span = basic_string_span<char32_t, Extent>;

template <std::ptrdiff_t Extent = dynamic_extent>
using cu32string_span = basic_string_span<const char32_t, Extent>;

//
// to_string() allow (explicit) conversions from string_span to string
//

template <typename CharT, std::ptrdiff_t Extent>
std::basic_string<typename std::remove_const<CharT>::type>
to_string(basic_string_span<CharT, Extent> view) {
  return {view.data(), narrow_cast<std::size_t>(view.length())};
}

template <typename CharT, typename Traits = typename std::char_traits<CharT>,
          typename Allocator = std::allocator<CharT>, typename gCharT,
          std::ptrdiff_t Extent>
std::basic_string<CharT, Traits, Allocator>
to_basic_string(basic_string_span<gCharT, Extent> view) {
  return {view.data(), narrow_cast<std::size_t>(view.length())};
}

template <class ElementType, std::ptrdiff_t Extent>
basic_string_span<const byte,
                  details::calculate_byte_size<ElementType, Extent>::value>
as_bytes(basic_string_span<ElementType, Extent> s) noexcept {
  GSL_SUPPRESS(type .1) // NO-FORMAT: attribute
  return {reinterpret_cast<const byte *>(s.data()), s.size_bytes()};
}

template <class ElementType, std::ptrdiff_t Extent,
          class = std::enable_if_t<!std::is_const<ElementType>::value>>
basic_string_span<byte,
                  details::calculate_byte_size<ElementType, Extent>::value>
as_writeable_bytes(basic_string_span<ElementType, Extent> s) noexcept {
  GSL_SUPPRESS(type .1) // NO-FORMAT: attribute
  return {reinterpret_cast<byte *>(s.data()), s.size_bytes()};
}

// zero-terminated string span, used to convert
// zero-terminated spans to legacy strings
template <typename CharT, std::ptrdiff_t Extent = dynamic_extent>
class basic_zstring_span {
public:
  using value_type = CharT;
  using const_value_type = std::add_const_t<CharT>;

  using pointer = std::add_pointer_t<value_type>;
  using const_pointer = std::add_pointer_t<const_value_type>;

  using zstring_type = basic_zstring<value_type, Extent>;
  using const_zstring_type = basic_zstring<const_value_type, Extent>;

  using impl_type = span<value_type, Extent>;
  using string_span_type = basic_string_span<value_type, Extent>;

  constexpr basic_zstring_span(impl_type s) : span_(s) {
    // expects a zero-terminated span
    Expects(s[s.size() - 1] == '\0');
  }

  // copy
  constexpr basic_zstring_span(const basic_zstring_span &other) = default;

  // move
  constexpr basic_zstring_span(basic_zstring_span &&other) = default;

  // assign
  constexpr basic_zstring_span &
  operator=(const basic_zstring_span &other) = default;

  // move assign
  constexpr basic_zstring_span &operator=(basic_zstring_span &&other) = default;

  constexpr bool empty() const noexcept { return span_.size() == 0; }

  constexpr string_span_type as_string_span() const noexcept {
    const auto sz = span_.size();
    return {span_.data(), sz > 1 ? sz - 1 : 0};
  }
  constexpr string_span_type ensure_z() const { return gsl::ensure_z(span_); }

  constexpr const_zstring_type assume_z() const noexcept {
    return span_.data();
  }

private:
  impl_type span_;
};

template <std::ptrdiff_t Max = dynamic_extent>
using zstring_span = basic_zstring_span<char, Max>;

template <std::ptrdiff_t Max = dynamic_extent>
using wzstring_span = basic_zstring_span<wchar_t, Max>;

template <std::ptrdiff_t Max = dynamic_extent>
using u16zstring_span = basic_zstring_span<char16_t, Max>;

template <std::ptrdiff_t Max = dynamic_extent>
using u32zstring_span = basic_zstring_span<char32_t, Max>;

template <std::ptrdiff_t Max = dynamic_extent>
using czstring_span = basic_zstring_span<const char, Max>;

template <std::ptrdiff_t Max = dynamic_extent>
using cwzstring_span = basic_zstring_span<const wchar_t, Max>;

template <std::ptrdiff_t Max = dynamic_extent>
using cu16zstring_span = basic_zstring_span<const char16_t, Max>;

template <std::ptrdiff_t Max = dynamic_extent>
using cu32zstring_span = basic_zstring_span<const char32_t, Max>;

// operator ==
template <class CharT, std::ptrdiff_t Extent, class T,
          class = std::enable_if_t<
              details::is_basic_string_span<T>::value ||
              std::is_convertible<
                  T, gsl::basic_string_span<std::add_const_t<CharT>>>::value>>
bool operator==(const gsl::basic_string_span<CharT, Extent> &one,
                const T &other) {
  const gsl::basic_string_span<std::add_const_t<CharT>> tmp(other);
  return std::equal(one.begin(), one.end(), tmp.begin(), tmp.end());
}

template <class CharT, std::ptrdiff_t Extent, class T,
          class = std::enable_if_t<
              !details::is_basic_string_span<T>::value &&
              std::is_convertible<
                  T, gsl::basic_string_span<std::add_const_t<CharT>>>::value>>
bool operator==(const T &one,
                const gsl::basic_string_span<CharT, Extent> &other) {
  const gsl::basic_string_span<std::add_const_t<CharT>> tmp(one);
  return std::equal(tmp.begin(), tmp.end(), other.begin(), other.end());
}

// operator !=
template <
    typename CharT, std::ptrdiff_t Extent = gsl::dynamic_extent, typename T,
    typename = std::enable_if_t<std::is_convertible<
        T, gsl::basic_string_span<std::add_const_t<CharT>, Extent>>::value>>
bool operator!=(gsl::basic_string_span<CharT, Extent> one, const T &other) {
  return !(one == other);
}

template <
    typename CharT, std::ptrdiff_t Extent = gsl::dynamic_extent, typename T,
    typename = std::enable_if_t<
        std::is_convertible<T, gsl::basic_string_span<std::add_const_t<CharT>,
                                                      Extent>>::value &&
        !gsl::details::is_basic_string_span<T>::value>>
bool operator!=(const T &one, gsl::basic_string_span<CharT, Extent> other) {
  return !(one == other);
}

// operator<
template <
    typename CharT, std::ptrdiff_t Extent = gsl::dynamic_extent, typename T,
    typename = std::enable_if_t<std::is_convertible<
        T, gsl::basic_string_span<std::add_const_t<CharT>, Extent>>::value>>
bool operator<(gsl::basic_string_span<CharT, Extent> one, const T &other) {
  const gsl::basic_string_span<std::add_const_t<CharT>, Extent> tmp(other);
  return std::lexicographical_compare(one.begin(), one.end(), tmp.begin(),
                                      tmp.end());
}

template <
    typename CharT, std::ptrdiff_t Extent = gsl::dynamic_extent, typename T,
    typename = std::enable_if_t<
        std::is_convertible<T, gsl::basic_string_span<std::add_const_t<CharT>,
                                                      Extent>>::value &&
        !gsl::details::is_basic_string_span<T>::value>>
bool operator<(const T &one, gsl::basic_string_span<CharT, Extent> other) {
  gsl::basic_string_span<std::add_const_t<CharT>, Extent> tmp(one);
  return std::lexicographical_compare(tmp.begin(), tmp.end(), other.begin(),
                                      other.end());
}

#ifndef _MSC_VER

// VS treats temp and const containers as convertible to basic_string_span,
// so the cases below are already covered by the previous operators

template <typename CharT, std::ptrdiff_t Extent = gsl::dynamic_extent,
          typename T, typename DataType = typename T::value_type,
          typename = std::enable_if_t<
              !gsl::details::is_span<T>::value &&
              !gsl::details::is_basic_string_span<T>::value &&
              std::is_convertible<DataType *, CharT *>::value &&
              std::is_same<std::decay_t<decltype(std::declval<T>().size(),
                                                 *std::declval<T>().data())>,
                           DataType>::value>>
bool operator<(gsl::basic_string_span<CharT, Extent> one, const T &other) {
  gsl::basic_string_span<std::add_const_t<CharT>, Extent> tmp(other);
  return std::lexicographical_compare(one.begin(), one.end(), tmp.begin(),
                                      tmp.end());
}

template <typename CharT, std::ptrdiff_t Extent = gsl::dynamic_extent,
          typename T, typename DataType = typename T::value_type,
          typename = std::enable_if_t<
              !gsl::details::is_span<T>::value &&
              !gsl::details::is_basic_string_span<T>::value &&
              std::is_convertible<DataType *, CharT *>::value &&
              std::is_same<std::decay_t<decltype(std::declval<T>().size(),
                                                 *std::declval<T>().data())>,
                           DataType>::value>>
bool operator<(const T &one, gsl::basic_string_span<CharT, Extent> other) {
  gsl::basic_string_span<std::add_const_t<CharT>, Extent> tmp(one);
  return std::lexicographical_compare(tmp.begin(), tmp.end(), other.begin(),
                                      other.end());
}
#endif

// operator <=
template <
    typename CharT, std::ptrdiff_t Extent = gsl::dynamic_extent, typename T,
    typename = std::enable_if_t<std::is_convertible<
        T, gsl::basic_string_span<std::add_const_t<CharT>, Extent>>::value>>
bool operator<=(gsl::basic_string_span<CharT, Extent> one, const T &other) {
  return !(other < one);
}

template <
    typename CharT, std::ptrdiff_t Extent = gsl::dynamic_extent, typename T,
    typename = std::enable_if_t<
        std::is_convertible<T, gsl::basic_string_span<std::add_const_t<CharT>,
                                                      Extent>>::value &&
        !gsl::details::is_basic_string_span<T>::value>>
bool operator<=(const T &one, gsl::basic_string_span<CharT, Extent> other) {
  return !(other < one);
}

#ifndef _MSC_VER

// VS treats temp and const containers as convertible to basic_string_span,
// so the cases below are already covered by the previous operators

template <typename CharT, std::ptrdiff_t Extent = gsl::dynamic_extent,
          typename T, typename DataType = typename T::value_type,
          typename = std::enable_if_t<
              !gsl::details::is_span<T>::value &&
              !gsl::details::is_basic_string_span<T>::value &&
              std::is_convertible<DataType *, CharT *>::value &&
              std::is_same<std::decay_t<decltype(std::declval<T>().size(),
                                                 *std::declval<T>().data())>,
                           DataType>::value>>
bool operator<=(gsl::basic_string_span<CharT, Extent> one, const T &other) {
  return !(other < one);
}

template <typename CharT, std::ptrdiff_t Extent = gsl::dynamic_extent,
          typename T, typename DataType = typename T::value_type,
          typename = std::enable_if_t<
              !gsl::details::is_span<T>::value &&
              !gsl::details::is_basic_string_span<T>::value &&
              std::is_convertible<DataType *, CharT *>::value &&
              std::is_same<std::decay_t<decltype(std::declval<T>().size(),
                                                 *std::declval<T>().data())>,
                           DataType>::value>>
bool operator<=(const T &one, gsl::basic_string_span<CharT, Extent> other) {
  return !(other < one);
}
#endif

// operator>
template <
    typename CharT, std::ptrdiff_t Extent = gsl::dynamic_extent, typename T,
    typename = std::enable_if_t<std::is_convertible<
        T, gsl::basic_string_span<std::add_const_t<CharT>, Extent>>::value>>
bool operator>(gsl::basic_string_span<CharT, Extent> one, const T &other) {
  return other < one;
}

template <
    typename CharT, std::ptrdiff_t Extent = gsl::dynamic_extent, typename T,
    typename = std::enable_if_t<
        std::is_convertible<T, gsl::basic_string_span<std::add_const_t<CharT>,
                                                      Extent>>::value &&
        !gsl::details::is_basic_string_span<T>::value>>
bool operator>(const T &one, gsl::basic_string_span<CharT, Extent> other) {
  return other < one;
}

#ifndef _MSC_VER

// VS treats temp and const containers as convertible to basic_string_span,
// so the cases below are already covered by the previous operators

template <typename CharT, std::ptrdiff_t Extent = gsl::dynamic_extent,
          typename T, typename DataType = typename T::value_type,
          typename = std::enable_if_t<
              !gsl::details::is_span<T>::value &&
              !gsl::details::is_basic_string_span<T>::value &&
              std::is_convertible<DataType *, CharT *>::value &&
              std::is_same<std::decay_t<decltype(std::declval<T>().size(),
                                                 *std::declval<T>().data())>,
                           DataType>::value>>
bool operator>(gsl::basic_string_span<CharT, Extent> one, const T &other) {
  return other < one;
}

template <typename CharT, std::ptrdiff_t Extent = gsl::dynamic_extent,
          typename T, typename DataType = typename T::value_type,
          typename = std::enable_if_t<
              !gsl::details::is_span<T>::value &&
              !gsl::details::is_basic_string_span<T>::value &&
              std::is_convertible<DataType *, CharT *>::value &&
              std::is_same<std::decay_t<decltype(std::declval<T>().size(),
                                                 *std::declval<T>().data())>,
                           DataType>::value>>
bool operator>(const T &one, gsl::basic_string_span<CharT, Extent> other) {
  return other < one;
}
#endif

// operator >=
template <
    typename CharT, std::ptrdiff_t Extent = gsl::dynamic_extent, typename T,
    typename = std::enable_if_t<std::is_convertible<
        T, gsl::basic_string_span<std::add_const_t<CharT>, Extent>>::value>>
bool operator>=(gsl::basic_string_span<CharT, Extent> one, const T &other) {
  return !(one < other);
}

template <
    typename CharT, std::ptrdiff_t Extent = gsl::dynamic_extent, typename T,
    typename = std::enable_if_t<
        std::is_convertible<T, gsl::basic_string_span<std::add_const_t<CharT>,
                                                      Extent>>::value &&
        !gsl::details::is_basic_string_span<T>::value>>
bool operator>=(const T &one, gsl::basic_string_span<CharT, Extent> other) {
  return !(one < other);
}

#ifndef _MSC_VER

// VS treats temp and const containers as convertible to basic_string_span,
// so the cases below are already covered by the previous operators

template <typename CharT, std::ptrdiff_t Extent = gsl::dynamic_extent,
          typename T, typename DataType = typename T::value_type,
          typename = std::enable_if_t<
              !gsl::details::is_span<T>::value &&
              !gsl::details::is_basic_string_span<T>::value &&
              std::is_convertible<DataType *, CharT *>::value &&
              std::is_same<std::decay_t<decltype(std::declval<T>().size(),
                                                 *std::declval<T>().data())>,
                           DataType>::value>>
bool operator>=(gsl::basic_string_span<CharT, Extent> one, const T &other) {
  return !(one < other);
}

template <typename CharT, std::ptrdiff_t Extent = gsl::dynamic_extent,
          typename T, typename DataType = typename T::value_type,
          typename = std::enable_if_t<
              !gsl::details::is_span<T>::value &&
              !gsl::details::is_basic_string_span<T>::value &&
              std::is_convertible<DataType *, CharT *>::value &&
              std::is_same<std::decay_t<decltype(std::declval<T>().size(),
                                                 *std::declval<T>().data())>,
                           DataType>::value>>
bool operator>=(const T &one, gsl::basic_string_span<CharT, Extent> other) {
  return !(one < other);
}
#endif
} // namespace gsl

#if defined(_MSC_VER) && !defined(__clang__)
#pragma warning(pop)

#if _MSC_VER < 1910
#undef constexpr
#pragma pop_macro("constexpr")

#endif // _MSC_VER < 1910
#endif // _MSC_VER

#endif // GSL_STRING_SPAN_H
//------------------------------------------------------------------------------
// END string_span
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
// Start renderer.h
//------------------------------------------------------------------------------

#include <functional>
#include <memory>
#include <vector>

namespace givr {

template <typename GeometryT, typename StyleT> struct RenderContext {
  std::unique_ptr<Program> shaderProgram;
  std::unique_ptr<VertexArray> vao;

  // Keep references to the GL_ARRAY_BUFFERS so that
  // the stay in scope for this context.
  std::vector<std::unique_ptr<Buffer>> arrayBuffers;

  GLuint numberOfIndices;
  GLuint startIndex;
  GLuint vertexCount;

  PrimitiveType primitive;

  bool hasIndices = false;

  typename StyleT::Parameters params;

  // Default ctor/dtor & move operations
  RenderContext() = default;
  ~RenderContext() = default;
  RenderContext(RenderContext &&) = default;
  RenderContext &operator=(RenderContext &&) = default;

  // But no copy or assignment. Bad.
  RenderContext(const RenderContext &) = delete;
  RenderContext &operator=(const RenderContext &) = delete;

  std::string getModelSource() const { return "uniform"; }
};

template <typename GeometryT, typename StyleT, typename ViewContextT>
void drawArray(
    RenderContext<GeometryT, StyleT> &ctx, ViewContextT const &viewCtx,
    std::function<void(std::unique_ptr<Program> const &)> setUniforms) {
  ctx.shaderProgram->use();

  mat4f view = viewCtx.camera.viewMatrix();
  mat4f projection = viewCtx.projection.projectionMatrix();
  vec3f viewPosition = viewCtx.camera.viewPosition();

  ctx.shaderProgram->setVec3("viewPosition", viewPosition);
  ctx.shaderProgram->setMat4("view", view);
  ctx.shaderProgram->setMat4("projection", projection);
  setUniforms(ctx.shaderProgram);
  ctx.vao->bind();
  glPolygonMode(GL_FRONT, GL_FILL);
  GLenum mode = givr::getMode(ctx.primitive);
  if constexpr (hasIndices<GeometryT>::value) {
    if (ctx.numberOfIndices > 0) {
      glDrawElements(mode, ctx.numberOfIndices, GL_UNSIGNED_INT, 0);
    } else {
      glDrawArrays(mode, ctx.startIndex, ctx.vertexCount);
    }
  } else {
    glDrawArrays(mode, ctx.startIndex, ctx.vertexCount);
  }

  ctx.vao->unbind();
}
template <typename GeometryT, typename StyleT>
void allocateBuffers(RenderContext<GeometryT, StyleT> &ctx) {
  ctx.vao = std::make_unique<VertexArray>();
  ctx.vao->alloc();

  if constexpr (hasIndices<GeometryT>::value) {
    // Map - but don't upload indices data
    std::unique_ptr<Buffer> indices = std::make_unique<Buffer>();
    indices->alloc();
    ctx.arrayBuffers.push_back(std::move(indices));
  }

  auto allocateBuffer = [&ctx]() {
    std::unique_ptr<Buffer> vbo = std::make_unique<Buffer>();
    vbo->alloc();
    ctx.arrayBuffers.push_back(std::move(vbo));
  };

  // Upload / bind / map model data
  if constexpr (hasVertices<GeometryT>::value) {
    allocateBuffer(); // data.vertices);
  }
  if constexpr (hasNormals<GeometryT>::value) {
    allocateBuffer(); // data.normals);
  }
  if constexpr (hasUvs<GeometryT>::value) {
    allocateBuffer(); // data.uvs);
  }
  if constexpr (hasColours<GeometryT>::value) {
    allocateBuffer(); // data.colours);
  }
}
template <typename GeometryT, typename StyleT>
void uploadBuffers(RenderContext<GeometryT, StyleT> &ctx,
                   typename GeometryT::Data const &data) {
  // Start by setting the appropriate context variables for rendering.
  if constexpr (hasIndices<GeometryT>::value) {
    ctx.numberOfIndices = data.indices.size();
  } else {
    ctx.numberOfIndices = 0;
  }
  ctx.startIndex = 0;
  ctx.vertexCount = data.vertices.size() / data.dimensions;

  std::uint16_t vaIndex = 4;
  ctx.vao->bind();

  std::uint16_t bufferIndex = 0;
  if constexpr (hasIndices<GeometryT>::value) {
    std::unique_ptr<Buffer> &indices = ctx.arrayBuffers[0];
    indices->bind(GL_ELEMENT_ARRAY_BUFFER);
    indices->data(GL_ELEMENT_ARRAY_BUFFER, data.indices,
                  getBufferUsageType(data.indicesType));
    ++bufferIndex;
  }

  auto applyBuffer = [&ctx, &vaIndex, &bufferIndex](
                         GLenum type, GLuint size, GLenum bufferType,
                         std::string name, gsl::span<const float> const &data) {
    // if this data piece is empty disable this one.
    std::unique_ptr<Buffer> &vbo = ctx.arrayBuffers[bufferIndex];
    vbo->bind(type);
    if (data.size() == 0) {
      glDisableVertexAttribArray(vaIndex);
    } else {
      glBindAttribLocation(*ctx.shaderProgram.get(), vaIndex, name.c_str());
      vbo->data(type, data, bufferType);
      glVertexAttribPointer(vaIndex, size, GL_FLOAT, GL_FALSE, 0, (GLvoid *)0);
      glEnableVertexAttribArray(vaIndex);
    }
    ++vaIndex;
    ++bufferIndex;
  };

  // Upload / bind / map model data
  if constexpr (hasVertices<GeometryT>::value) {
    applyBuffer(GL_ARRAY_BUFFER, data.dimensions,
                getBufferUsageType(data.verticesType), "position",
                data.vertices);
  }
  if constexpr (hasNormals<GeometryT>::value) {
    applyBuffer(GL_ARRAY_BUFFER, data.dimensions,
                getBufferUsageType(data.normalsType), "normals", data.normals);
  }
  if constexpr (hasUvs<GeometryT>::value) {
    applyBuffer(GL_ARRAY_BUFFER, 2, getBufferUsageType(data.uvsType), "uvs",
                data.uvs);
  }
  if constexpr (hasColours<GeometryT>::value) {
    applyBuffer(GL_ARRAY_BUFFER, 3, getBufferUsageType(data.coloursType),
                "colour", data.colours);
  }

  ctx.vao->unbind();

  if constexpr (hasIndices<GeometryT>::value) {
    ctx.arrayBuffers[0]->unbind(GL_ELEMENT_ARRAY_BUFFER);
    if (ctx.arrayBuffers.size() > 1) {
      ctx.arrayBuffers[1]->unbind(GL_ARRAY_BUFFER);
    }
  }
}
}; // end namespace givr
//------------------------------------------------------------------------------
// END renderer.h
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
// Start instanced_renderer.h
//------------------------------------------------------------------------------

#include <functional>
#include <memory>
#include <vector>

namespace givr {

template <typename GeometryT, typename StyleT> struct InstancedRenderContext {
  std::unique_ptr<Program> shaderProgram;
  std::unique_ptr<VertexArray> vao;

  std::vector<mat4f> modelTransforms;
  std::unique_ptr<Buffer> modelTransformsBuffer;

  // Keep references to the GL_ARRAY_BUFFERS so that
  // the stay in scope for this context.
  std::vector<std::unique_ptr<Buffer>> arrayBuffers;

  GLuint numberOfIndices;
  GLuint startIndex;
  GLuint vertexCount;

  PrimitiveType primitive;

  typename StyleT::Parameters params;

  // Default ctor/dtor & move operations
  InstancedRenderContext() = default;
  ~InstancedRenderContext() = default;
  InstancedRenderContext(InstancedRenderContext &&) = default;
  InstancedRenderContext &operator=(InstancedRenderContext &&) = default;

  // But no copy or assignment. Bad.
  InstancedRenderContext(const InstancedRenderContext &) = delete;
  InstancedRenderContext &operator=(const InstancedRenderContext &) = delete;

  std::string getModelSource() { return "layout(location=0) in "; }
};

template <typename GeometryT, typename StyleT, typename ViewContextT>
void drawInstanced(
    InstancedRenderContext<GeometryT, StyleT> &ctx, ViewContextT const &viewCtx,
    std::function<void(std::unique_ptr<Program> const &)> setUniforms) {
  ctx.shaderProgram->use();

  mat4f view = viewCtx.camera.viewMatrix();
  mat4f projection = viewCtx.projection.projectionMatrix();
  vec3f viewPosition = viewCtx.camera.viewPosition();

  ctx.shaderProgram->setVec3("viewPosition", viewPosition);
  ctx.shaderProgram->setMat4("view", view);
  ctx.shaderProgram->setMat4("projection", projection);
  setUniforms(ctx.shaderProgram);

  ctx.vao->bind();
  glPolygonMode(GL_FRONT, GL_FILL);
  GLenum mode = givr::getMode(ctx.primitive);
  ctx.modelTransformsBuffer->bind(GL_ARRAY_BUFFER);
  ctx.modelTransformsBuffer->data(
      GL_ARRAY_BUFFER, gsl::span<mat4f>(ctx.modelTransforms), GL_DYNAMIC_DRAW);

  if constexpr (hasIndices<GeometryT>::value) {
    if (ctx.numberOfIndices > 0) {
      glDrawElementsInstanced(mode, ctx.numberOfIndices, GL_UNSIGNED_INT, 0,
                              ctx.modelTransforms.size());
    } else {
      glDrawArraysInstanced(mode, ctx.startIndex, ctx.vertexCount,
                            ctx.modelTransforms.size());
    }
  } else {
    glDrawArraysInstanced(mode, ctx.startIndex, ctx.vertexCount,
                          ctx.modelTransforms.size());
  }

  ctx.vao->unbind();

  ctx.modelTransforms.clear();
}

template <typename GeometryT, typename StyleT>
void allocateBuffers(InstancedRenderContext<GeometryT, StyleT> &ctx) {
  ctx.vao = std::make_unique<VertexArray>();
  ctx.vao->alloc();

  // Map - but don't upload framing data.
  ctx.modelTransformsBuffer = std::make_unique<Buffer>();
  ctx.modelTransformsBuffer->alloc();

  if constexpr (hasIndices<GeometryT>::value) {
    // Map - but don't upload indices data
    std::unique_ptr<Buffer> indices = std::make_unique<Buffer>();
    indices->alloc();
    ctx.arrayBuffers.push_back(std::move(indices));
  }

  auto allocateBuffer = [&ctx]() {
    std::unique_ptr<Buffer> vbo = std::make_unique<Buffer>();
    vbo->alloc();
    ctx.arrayBuffers.push_back(std::move(vbo));
  };

  // Upload / bind / map model data
  if constexpr (hasVertices<GeometryT>::value) {
    allocateBuffer(); // data.vertices);
  }
  if constexpr (hasNormals<GeometryT>::value) {
    allocateBuffer(); // data.normals);
  }
  if constexpr (hasUvs<GeometryT>::value) {
    allocateBuffer(); // data.uvs);
  }
  if constexpr (hasColours<GeometryT>::value) {
    allocateBuffer(); // data.colours);
  }
}

template <typename GeometryT, typename StyleT>
void uploadBuffers(InstancedRenderContext<GeometryT, StyleT> &ctx,
                   typename GeometryT::Data const &data) {
  // Start by setting the appropriate context variables for rendering.
  if constexpr (hasIndices<GeometryT>::value) {
    ctx.numberOfIndices = data.indices.size();
  } else {
    ctx.numberOfIndices = 0;
  }
  ctx.startIndex = 0;
  ctx.vertexCount = data.vertices.size() / data.dimensions;

  std::uint16_t vaIndex = 0;
  ctx.vao->bind();

  // Upload framing data.
  ctx.modelTransformsBuffer->bind(GL_ARRAY_BUFFER);
  auto vec4Size = sizeof(mat4f) / 4;
  for (std::uint16_t i = 0; i < 4; ++i) {
    glVertexAttribPointer(vaIndex, 4, GL_FLOAT, GL_FALSE, sizeof(mat4f),
                          (GLvoid *)(i * vec4Size));
    glEnableVertexAttribArray(vaIndex);
    glVertexAttribDivisor(vaIndex, 1);
    ++vaIndex;
  }

  std::uint16_t bufferIndex = 0;
  if constexpr (hasIndices<GeometryT>::value) {
    std::unique_ptr<Buffer> &indices = ctx.arrayBuffers[0];
    indices->bind(GL_ELEMENT_ARRAY_BUFFER);
    indices->data(GL_ELEMENT_ARRAY_BUFFER, data.indices,
                  getBufferUsageType(data.indicesType));
    ++bufferIndex;
  }

  auto applyBuffer = [&ctx, &vaIndex, &bufferIndex](
                         GLenum type, GLuint size, GLenum bufferType,
                         std::string name, gsl::span<const float> const &data) {
    std::unique_ptr<Buffer> &vbo = ctx.arrayBuffers[bufferIndex];
    vbo->bind(type);
    if (data.size() == 0) {
      glDisableVertexAttribArray(vaIndex);
    } else {
      vbo->data(type, data, bufferType);
      glBindAttribLocation(*ctx.shaderProgram.get(), vaIndex, name.c_str());
      glVertexAttribPointer(vaIndex, size, GL_FLOAT, GL_FALSE, 0, (GLvoid *)0);
      glEnableVertexAttribArray(vaIndex);
    }
    ++vaIndex;
    ++bufferIndex;
  };

  // Upload / bind / map model data
  if constexpr (hasVertices<GeometryT>::value)
    applyBuffer(GL_ARRAY_BUFFER, data.dimensions,
                getBufferUsageType(data.verticesType), "position",
                data.vertices);
  if constexpr (hasNormals<GeometryT>::value)
    applyBuffer(GL_ARRAY_BUFFER, data.dimensions,
                getBufferUsageType(data.normalsType), "normals", data.normals);
  if constexpr (hasUvs<GeometryT>::value)
    applyBuffer(GL_ARRAY_BUFFER, 2, getBufferUsageType(data.uvsType), "uvs",
                data.uvs);
  if constexpr (hasColours<GeometryT>::value)
    applyBuffer(GL_ARRAY_BUFFER, 3, getBufferUsageType(data.coloursType),
                "colour", data.colours);

  ctx.vao->unbind();

  ctx.arrayBuffers[0]->unbind(GL_ELEMENT_ARRAY_BUFFER);
  if (ctx.arrayBuffers.size() > 1) {
    ctx.arrayBuffers[1]->unbind(GL_ARRAY_BUFFER);
  }
}
}; // end namespace givr
//------------------------------------------------------------------------------
// END instanced_renderer.h
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
// Start triangle_soup.h
//------------------------------------------------------------------------------
#include <vector>

namespace givr {
namespace geometry {

struct TriangleSoup {
private:
  std::vector<TriangleGeometry> m_triangles;

public:
  template <typename... Args> TriangleSoup(Args &&... args) {
    std::vector<TriangleGeometry> tris{args...};
    for (auto &t : tris) {
      m_triangles.push_back(t);
    }
  }

  std::vector<TriangleGeometry> &triangles() { return m_triangles; }
  std::vector<TriangleGeometry> const &triangles() const { return m_triangles; }

  void push_back(TriangleGeometry t) { m_triangles.push_back(t); }
  void push_back(vec3f const &p1, vec3f const &p2, vec3f const &p3) {
    m_triangles.push_back(Triangle(Point1(p1), Point2(p2), Point3(p3)));
  }

  struct Data : public VertexArrayData<PrimitiveType::TRIANGLES> {
    std::uint16_t dimensions = 3;

    BufferUsageType verticesType = BufferUsageType::STATIC_DRAW;
    std::vector<float> vertices;

    BufferUsageType normalsType = BufferUsageType::STATIC_DRAW;
    std::vector<float> normals;
  };
};
// Backwards Compatibility
using TriangleSoupGeometry = TriangleSoup;

TriangleSoup::Data generateGeometry(TriangleSoup const &t);
} // end namespace geometry
} // end namespace givr
//------------------------------------------------------------------------------
// END triangle_soup.h
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
// Start multiline.h
//------------------------------------------------------------------------------
#include <vector>

namespace givr {
namespace geometry {

struct MultiLine {
private:
  std::vector<Line> m_segments;

public:
  template <typename... Args>
  MultiLine(Args &&... args) : m_segments{std::forward<Args>(args)...} {}

  std::vector<Line> &segments() { return m_segments; }
  std::vector<Line> const &segments() const { return m_segments; }
  std::vector<Line> &operator*() { return m_segments; }

  void push_back(Line l);

  struct Data : public VertexArrayData<PrimitiveType::LINES> {
    std::uint16_t dimensions = 3;

    BufferUsageType verticesType = BufferUsageType::STATIC_DRAW;
    std::vector<float> vertices;
  };
};
// Backwards Compatibility
using MultiLineGeometry = MultiLine;

MultiLine::Data generateGeometry(MultiLine const &l);
} // end namespace geometry
} // end namespace givr
//------------------------------------------------------------------------------
// END multiline.h
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
// Start gsl
//------------------------------------------------------------------------------
///////////////////////////////////////////////////////////////////////////////
//
// Copyright (c) 2015 Microsoft Corporation. All rights reserved.
//
// This code is licensed under the MIT License (MIT).
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
// THE SOFTWARE.
//
///////////////////////////////////////////////////////////////////////////////

#ifndef GSL_GSL_H
#define GSL_GSL_H

#endif // GSL_GSL_H
//------------------------------------------------------------------------------
// END gsl
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
// Start draw.h
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
// Example Code:
// FlatShading style{.colour=yellow};
// Sphere geom{.radius=1};
// auto spheres = createInstancedRenderable(geom, style);
// for (..) {
//     addInstance(spheres, at(x, y));
// }
// draw(spheres, view);
//------------------------------------------------------------------------------
namespace givr {
template <typename GeometryT, typename StyleT>
InstancedRenderContext<GeometryT, StyleT>
createInstancedRenderable(GeometryT const &g, StyleT const &style) {
  auto ctx = getInstancedContext(g, style);
  allocateBuffers(ctx);
  uploadBuffers(ctx, fillBuffers(g, style));
  return ctx;
}
template <typename GeometryT, typename StyleT>
RenderContext<GeometryT, StyleT> createRenderable(GeometryT const &g,
                                                  StyleT const &style) {
  auto ctx = getContext(g, style);
  allocateBuffers(ctx);
  uploadBuffers(ctx, fillBuffers(g, style));
  return ctx;
}
template <typename GeometryT, typename StyleT>
void updateRenderable(GeometryT const &g, StyleT const &style,
                      InstancedRenderContext<GeometryT, StyleT> &ctx) {
  updateStyle(ctx, style);
  uploadBuffers(ctx, fillBuffers(g, style));
}
template <typename GeometryT, typename StyleT>
void updateRenderable(GeometryT const &g, StyleT const &style,
                      RenderContext<GeometryT, StyleT> &ctx) {
  updateStyle(ctx, style);
  uploadBuffers(ctx, fillBuffers(g, style));
}
template <typename GeometryT, typename StyleT>
void addInstance(InstancedRenderContext<GeometryT, StyleT> &ctx,
                 glm::mat4 const &f) {
  ctx.modelTransforms.push_back(f);
}

} // namespace givr
//------------------------------------------------------------------------------
// END draw.h
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
// Start lines.h
//------------------------------------------------------------------------------

#include <string>

namespace givr {
namespace style {
struct GL_LineParameters : public Style<Colour, Width> {};

struct GL_Line : public GL_LineParameters {
  using Parameters = GL_LineParameters;
  template <typename... Args> GL_Line(Args &&... args) {
    using required_args = std::tuple<Colour>;

    using namespace utility;
    static_assert(!has_duplicate_types<Args...>,
                  "The arguments you passed in have duplicate parameters");

    static_assert(
        is_subset_of<required_args, std::tuple<Args...>>,
        "Colour is a required parameter for GL_Line. Please provide it.");
    static_assert(is_subset_of<std::tuple<Args...>, GL_Line::Args>,
                  "You have provided incorrect parameters for GL_Line. "
                  "Colour is required. Width is optional.");
    static_assert(sizeof...(args) <= std::tuple_size<GL_Line::Args>::value,
                  "You have provided incorrect parameters for GL_Line. "
                  "Colour is required. Width is optional.");
    set(Width(1.0));
    set(std::forward<Args>(args)...);
  }
};

using LineStyle = GL_Line; // Backwards compatibility

template <typename GeometryT>
typename GeometryT::Data fillBuffers(GeometryT const &g, GL_Line const &) {
  static_assert(givr::isLineBased<GeometryT>(),
                R"error(
            The GL_Line style requires LINES, LINE_LOOP, LINE_STRIP,
            LINES_ADJACENCY, or LINE_STRIP_ADJACENCY for the primitive
            type. The geometry you use is not of this type"
            )error");
  static_assert(hasVertices<GeometryT>::value,
                R"error(
            The GL_Line style requires vertices. The geometry you are using
            does not provide them.
            )error");
  return std::move(generateGeometry(g));
}

template <typename RenderContextT>
void setLineUniforms(RenderContextT const &ctx,
                     std::unique_ptr<givr::Program> const &p) {
  p->setVec3("colour", ctx.params.template value<Colour>());
}
std::string linesVertexSource(std::string modelSource);
std::string linesFragmentSource();

template <typename GeometryT>
RenderContext<GeometryT, GL_Line> getContext(GeometryT const &,
                                             GL_Line const &l) {
  RenderContext<GeometryT, GL_Line> ctx;
  ctx.shaderProgram = std::make_unique<Program>(
      Shader{linesVertexSource(ctx.getModelSource()), GL_VERTEX_SHADER},
      Shader{linesFragmentSource(), GL_FRAGMENT_SHADER});
  ctx.primitive = getPrimitive<GeometryT>();
  updateStyle(ctx, l);
  return ctx;
}

template <typename RenderContextT>
void updateStyle(RenderContextT &ctx, GL_Line const &l) {
  ctx.params.set(l.args);
}

template <typename GeometryT, typename ViewContextT>
void draw(InstancedRenderContext<GeometryT, GL_Line> &ctx,
          ViewContextT const &viewCtx) {
  glEnable(GL_LINE_SMOOTH);
  glLineWidth(ctx.params.template value<Width>());
  drawInstanced(ctx, viewCtx, [&ctx](std::unique_ptr<Program> const &program) {
    setLineUniforms(ctx, program);
  });
}

template <typename GeometryT, typename ViewContextT>
void draw(RenderContext<GeometryT, GL_Line> &ctx, ViewContextT const &viewCtx,
          mat4f model = mat4f(1.f)) {
  glEnable(GL_LINE_SMOOTH);
  glLineWidth(ctx.params.template value<Width>());
  drawArray(ctx, viewCtx,
            [&ctx, &model](std::unique_ptr<Program> const &program) {
              setLineUniforms(ctx, program);
              program->setMat4("model", model);
            });
}

} // namespace style
} // namespace givr
//------------------------------------------------------------------------------
// END lines.h
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
// Start noshading.h
//------------------------------------------------------------------------------

#include <string>

namespace givr {
namespace style {
struct NoShadingParameters : public Style<Colour> {};

struct NoShading : public NoShadingParameters {
  using Parameters = NoShadingParameters;
  template <typename... Args> NoShading(Args &&... args) {
    using required_args = std::tuple<Colour>;

    using namespace utility;
    static_assert(!has_duplicate_types<Args...>,
                  "The arguments you passed in have duplicate parameters");

    static_assert(
        is_subset_of<required_args, std::tuple<Args...>>,
        "Colour is a required parameter for NoShading. Please provide it.");
    static_assert(is_subset_of<std::tuple<Args...>, NoShading::Args>,
                  "You have provided incorrect parameters for NoShading. "
                  "Colour is required.");
    static_assert(sizeof...(args) <= std::tuple_size<NoShading::Args>::value,
                  "You have provided incorrect parameters for NoShading. "
                  "Colour is required.");

    set(std::forward<Args>(args)...);
  }
};
using NoShadingStyle = NoShading;

template <typename RenderContextT>
void setNoShadingUniforms(RenderContextT const &ctx,
                          std::unique_ptr<givr::Program> const &p) {
  p->setVec3("colour", ctx.params.template value<givr::style::Colour>());
}

std::string noShadingVertexSource(std::string modelSource);
std::string noShadingFragmentSource();

template <typename GeometryT>
typename GeometryT::Data fillBuffers(GeometryT const &g, NoShading const &) {
  static_assert(hasVertices<GeometryT>::value, "The NoShading style requires "
                                               "vertices. The geometry you are "
                                               "using does not provide them.");
  return std::move(generateGeometry(g));
}

template <typename GeometryT>
RenderContext<GeometryT, NoShading> getContext(GeometryT const &,
                                               NoShading const &f) {
  std::cout << "NoShading" << std::endl;
  RenderContext<GeometryT, NoShading> ctx;
  ctx.shaderProgram = std::make_unique<Program>(
      Shader{noShadingVertexSource(ctx.getModelSource()), GL_VERTEX_SHADER},
      Shader{noShadingFragmentSource(), GL_FRAGMENT_SHADER});
  ctx.primitive = getPrimitive<GeometryT>();
  updateStyle(ctx, f);
  return ctx;
}

template <typename RenderContextT>
void updateStyle(RenderContextT &ctx, NoShading const &f) {
  ctx.params.set(f.args);
}

template <typename GeometryT, typename ViewContextT>
void draw(InstancedRenderContext<GeometryT, NoShading> &ctx,
          ViewContextT const &viewCtx) {
  drawInstanced(ctx, viewCtx, [&ctx](std::unique_ptr<Program> const &program) {
    setNoShadingUniforms(ctx, program);
  });
}

template <typename GeometryT, typename ViewContextT>
void draw(RenderContext<GeometryT, NoShading> &ctx, ViewContextT const &viewCtx,
          mat4f model = mat4f(1.f)) {
  drawArray(ctx, viewCtx,
            [&ctx, &model](std::unique_ptr<Program> const &program) {
              setNoShadingUniforms(ctx, program);
              program->setMat4("model", model);
            });
}
} // end namespace style
} // end namespace givr
//------------------------------------------------------------------------------
// END noshading.h
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
// Start phong.h
//------------------------------------------------------------------------------

#include <string>

namespace givr {
namespace style {
template <typename ColorSrc>
struct T_PhongParameters
    : public Style<ColorSrc, LightPosition, SpecularFactor, AmbientFactor,
                   PhongExponent, PerVertexColour, ShowWireFrame,
                   WireFrameColour, WireFrameWidth, GenerateNormals> {};

template <typename ColorSrc> struct T_Phong : T_PhongParameters<ColorSrc> {
  using Parameters = T_PhongParameters<ColorSrc>;
  template <typename... T_PhongArgs> T_Phong(T_PhongArgs &&... args) {
    using required_args = std::tuple<LightPosition, ColorSrc>;

    using namespace style;
    using namespace utility;

    static_assert(!has_duplicate_types<T_PhongArgs...>,
                  "The arguments you passed in have duplicate parameters");

    static_assert(
        is_subset_of<required_args, std::tuple<T_PhongArgs...>> &&
            is_subset_of<std::tuple<T_PhongArgs...>,
                         typename T_Phong<ColorSrc>::Args> &&
            sizeof...(args) <=
                std::tuple_size<typename T_Phong<ColorSrc>::Args>::value,
        "You have provided incorrect parameters for phong. "
        "LightPosition and (Colour or ColorTexture) are required "
        "AmbientFactor, SpecularFactor PhongExponent and PerVertexColor "
        "are optional.");

    this->set(PerVertexColour(false));
    this->set(AmbientFactor(0.05f));
    this->set(SpecularFactor(0.3f));
    this->set(PhongExponent(8.0f));
    this->set(ShowWireFrame(false));
    this->set(WireFrameColour(0.f, 0.f, 0.f));
    this->set(WireFrameWidth(1.5f));
    this->set(GenerateNormals(false));
    this->set(std::forward<T_PhongArgs>(args)...);
  }
};

std::string phongVertexSource(std::string modelSource, bool usingTexture,
                              bool hasNormals, bool hasColours);
std::string phongGeometrySource(bool usingTexture, bool hasNormals,
                                bool hasColours);
std::string phongFragmentSource(bool usingTexture, bool hasColours);

template <typename RenderContextT>
void setPhongUniforms(RenderContextT const &ctx,
                      std::unique_ptr<givr::Program> const &p) {
  using namespace givr::style;
  if constexpr (std::is_same<RenderContextT, T_Phong<ColorTexture>>::value) {
    givr::Texture texture = ctx.template value<ColorTexture>();
    if (GLuint(texture)) {
      glActiveTexture(GL_TEXTURE1);
      texture.bind(GL_TEXTURE_2D);
      p->setInt("colorTexture", 1);
      glActiveTexture(GL_TEXTURE0);
    }
  } else {
    p->setVec3("colour", ctx.params.template value<Colour>());
  }
  p->setVec3("lightPosition", ctx.params.template value<LightPosition>());
  p->setFloat("ambientFactor", ctx.params.template value<AmbientFactor>());
  p->setFloat("specularFactor", ctx.params.template value<SpecularFactor>());
  p->setFloat("phongExponent", ctx.params.template value<PhongExponent>());
  p->setBool("perVertexColour", ctx.params.template value<PerVertexColour>());
  p->setBool("showWireFrame", ctx.params.template value<ShowWireFrame>());
  p->setVec3("wireFrameColour", ctx.params.template value<WireFrameColour>());
  p->setFloat("wireFrameWidth", ctx.params.template value<WireFrameWidth>());
  p->setBool("generateNormals", ctx.params.template value<GenerateNormals>());
}

template <typename GeometryT, typename ColorSrc>
typename GeometryT::Data fillBuffers(GeometryT const &g,
                                     T_Phong<ColorSrc> const &) {
  static_assert(
      givr::isTriangleBased<GeometryT>(),
      "The PhongStyle requires TRIANGLES, TRIANGLE_STRIP, TRIANGLE_FAN, "
      "TRIANGLES_ADJACENCY, or TRIANGLE_STRIP_ADJACENCY for the primitive "
      "type. The geometry you use is not of this type");
  static_assert(
      hasVertices<GeometryT>::value,
      "The Phong style requires vertices. The geometry you are using does "
      "not provide them.");
  // static_assert(
  // hasNormals<GeometryT>::value,
  //"The Phong style requires normals. The geometry you are using does "
  //"not provide them."
  //);
  static_assert(
      hasUvs<GeometryT>::value || std::is_same<ColorSrc, Colour>::value,
      "The Phong Texture style requires uvs. The geometry you are using does "
      "not provide them.");

  return std::move(generateGeometry(g));
}

template <typename GeometryT, typename StyleT>
std::unique_ptr<Program> getPhongShaderProgram(std::string modelSource) {
  constexpr bool _hasNormals = hasNormals<GeometryT>::value;
  constexpr bool _hasColours = hasColours<GeometryT>::value;
  constexpr bool _useTex = std::is_same<StyleT, T_Phong<ColorTexture>>::value;
  return std::make_unique<Program>(
      Shader{phongVertexSource(modelSource, _useTex, _hasNormals, _hasColours),
             GL_VERTEX_SHADER},
      Shader{phongGeometrySource(_useTex, _hasNormals, _hasColours),
             GL_GEOMETRY_SHADER},
      Shader{phongFragmentSource(_useTex, _hasColours), GL_FRAGMENT_SHADER});
}

template <typename GeometryT, typename ColorSrc>
RenderContext<GeometryT, T_Phong<ColorSrc>>
getContext(GeometryT const &, T_Phong<ColorSrc> const &p) {
  std::cout << "Phong" << std::endl;
  RenderContext<GeometryT, T_Phong<ColorSrc>> ctx;
  ctx.shaderProgram =
      getPhongShaderProgram<GeometryT, T_Phong<ColorSrc>>(ctx.getModelSource());
  ctx.primitive = getPrimitive<GeometryT>();
  updateStyle(ctx, p);
  return std::move(ctx);
}

template <typename GeometryT, typename StyleT>
InstancedRenderContext<GeometryT, StyleT> getInstancedContext(GeometryT const &,
                                                              StyleT const &p) {
  InstancedRenderContext<GeometryT, StyleT> ctx;
  ctx.shaderProgram =
      getPhongShaderProgram<GeometryT, StyleT>(ctx.getModelSource());
  ctx.primitive = getPrimitive<GeometryT>();
  updateStyle(ctx, p);
  return std::move(ctx);
}

template <typename RenderContextT, typename ColorSrc>
void updateStyle(RenderContextT &ctx, T_Phong<ColorSrc> const &p) {
  ctx.params.set(p.args);
}

// TODO: come up with a better way to not duplicate OpenGL state setup
template <typename GeometryT, typename ViewContextT, typename ColorSrc>
void draw(InstancedRenderContext<GeometryT, T_Phong<ColorSrc>> &ctx,
          ViewContextT const &viewCtx) {
  glEnable(GL_MULTISAMPLE);
  glEnable(GL_DEPTH_TEST);
  glEnable(GL_BLEND);
  glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
  drawInstanced(ctx, viewCtx, [&ctx](std::unique_ptr<Program> const &program) {
    setPhongUniforms(ctx, program);
  });
}

template <typename GeometryT, typename ViewContextT, typename ColorSrc>
void draw(RenderContext<GeometryT, T_Phong<ColorSrc>> &ctx,
          ViewContextT const &viewCtx, mat4f const model = mat4f(1.f)) {
  glEnable(GL_MULTISAMPLE);
  glEnable(GL_DEPTH_TEST);
  glEnable(GL_BLEND);
  glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
  drawArray(ctx, viewCtx,
            [&ctx, &model](std::unique_ptr<Program> const &program) {
              setPhongUniforms(ctx, program);
              program->setMat4("model", model);
            });
}

using TexturedPhongStyle = T_Phong<ColorTexture>;
using PhongStyle = T_Phong<Colour>;
using TexturedPhong = T_Phong<ColorTexture>;
using Phong = T_Phong<Colour>;

} // end namespace style
} // end namespace givr

//------------------------------------------------------------------------------
// END phong.h
//------------------------------------------------------------------------------
