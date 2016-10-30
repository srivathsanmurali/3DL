#ifndef _PLYIO_H_
#define _PLYIO_H_

//STL
#include <cstdlib>
#include <vector>
#include <map>
#include <sstream>
#include <fstream>

// pcLib
#include <common.h>
#include <types.h>


/*! brief Possible Ply Formats
 *
 *  Ascii or Binary
 */
enum PlyFormats {
  kAscii        = 0,
  kLittleEndian = 1,
  kBigEndian    = 2
};

/*! brief Possible element types
 *
 *  Vertex or Face
 */
enum PlyElementTypes {
  kVertex       = 0,
  kFace         = 1,
};

/*! brief Possible Property types
 *
 *  Indentifying various property types supported by elements
 */
enum PlyPropertyTypes{
  kInvalid       = 0,
  kX             = 1,
  kY             = 2,
  kZ             = 3,
  kNX            = 4,
  kNY            = 5,
  kNZ            = 6,
  kR             = 7,
  kG             = 8,
  kB             = 9,
  kA             = 10,
  kListInd       = 11,
  kListTexCoords = 12,
  kFlags         = 13,
};

/// Provides map between property type and string name
static std::map<PlyPropertyTypes, std::string> PropertyTypeTable {
    {PlyPropertyTypes::kInvalid       , "invalid"},
    {PlyPropertyTypes::kX             , "x"},
    {PlyPropertyTypes::kY             , "y"},
    {PlyPropertyTypes::kZ             , "z"},
    {PlyPropertyTypes::kNX            , "nx"},
    {PlyPropertyTypes::kNY            , "ny"},
    {PlyPropertyTypes::kNZ            , "nz"},
    {PlyPropertyTypes::kR             , "red"},
    {PlyPropertyTypes::kG             , "green"},
    {PlyPropertyTypes::kB             , "blue"},
    {PlyPropertyTypes::kA             , "alpha"},
    {PlyPropertyTypes::kListInd       , "vertex_index"},
    {PlyPropertyTypes::kListTexCoords , "texcoord"},
    {PlyPropertyTypes::kFlags         , "flags"}
};

/// brief Find property type from string property name
inline PlyPropertyTypes propertyTypeFromString (const std::string & t) {
  if      (t == "x")                                     return PlyPropertyTypes::kX;
  else if (t == "y")                                     return PlyPropertyTypes::kY;
  else if (t == "z")                                     return PlyPropertyTypes::kZ;
  else if (t == "nx" || t == "normal_x")                 return PlyPropertyTypes::kNX;
  else if (t == "ny" || t == "normal_y")                 return PlyPropertyTypes::kNY;
  else if (t == "nz" || t == "normal_z")                 return PlyPropertyTypes::kNZ;
  else if (t == "red"   || t == "r")                     return PlyPropertyTypes::kR;
  else if (t == "green" || t == "g")                     return PlyPropertyTypes::kG;
  else if (t == "blue"  || t == "b")                     return PlyPropertyTypes::kB;
  else if (t == "alpha" || t == "a")                     return PlyPropertyTypes::kA;
  else if (t == "vertex_index" || t == "vertex_indices") return PlyPropertyTypes::kListInd;
  else if (t == "texcoord")                              return PlyPropertyTypes::kListTexCoords;
  else if (t == "flags")                                 return PlyPropertyTypes::kFlags;
  else return PlyPropertyTypes::kInvalid;
}

/// Variable types supported
enum class PlyTypes: uint8_t {
  INVALID,
  INT8,
  UINT8,
  INT16,
  UINT16,
  INT32,
  UINT32,
  FLOAT32,
  FLOAT64,
};

struct PropertyInfo { int stride; std::string str; };

/// Maps from Property type to stride and name
static std::map<PlyTypes , PropertyInfo> TypeTable{
    {PlyTypes::INT8,       {1, "char"}},
    {PlyTypes::UINT8,      {1, "uchar"}},
    {PlyTypes::INT16,      {2, "short"}},
    {PlyTypes::UINT16,     {2, "ushort"}},
    {PlyTypes::INT32,      {4, "int"}},
    {PlyTypes::UINT32,     {4, "uint"}},
    {PlyTypes::FLOAT32,    {4, "float"}},
    {PlyTypes::FLOAT64,    {8, "double"}},
    {PlyTypes::INVALID,    {0, "INVALID"}}
};

/// find variable type from string
inline PlyTypes variableTypeFromString(const std::string & t) {
  if      (t == "int8"    || t == "char")     return PlyTypes::INT8;
  else if (t == "uint8"   || t == "uchar")    return PlyTypes::UINT8;
  else if (t == "int16"   || t == "short")    return PlyTypes::INT16;
  else if (t == "uint16"  || t == "ushort")   return PlyTypes::UINT16;
  else if (t == "int32"   || t == "int")      return PlyTypes::INT32;
  else if (t == "uint32"  || t == "uint")     return PlyTypes::UINT32;
  else if (t == "float32" || t == "float")    return PlyTypes::FLOAT32;
  else if (t == "float64" || t == "double")
    throwRuntimeError("pcLib doesnt support double types yet");
    //return PlyTypes::FLOAT64; // doesnt support double yet
  return PlyTypes::INVALID;
}


/** \struct PlyProperty
  * \brief Provides information about each property in
  *        elements.
  */
struct PlyProperty {
  PlyPropertyTypes  propertyType;
  PlyTypes          variableType;
  bool              isList;
  PlyTypes          listSizeType;
  int               size;

  PlyProperty() {}
  PlyProperty (std::istringstream & is) {
    isList = false;
    std::string pVarType, pType;
    is >> pVarType;
    if(pVarType == "list" || pVarType == "List"){
      isList = true;
      std::string pListType;
      is >> pListType;
      listSizeType = variableTypeFromString(pListType);
      if(listSizeType == PlyTypes::INVALID)
        throwRuntimeError("Invalid list size type " + pListType);
      is >> pVarType;
    }
    is >> pType;

    variableType = variableTypeFromString(pVarType);
    if(variableType == PlyTypes::INVALID)
      throwRuntimeError("Invalid variable type " + pVarType);

    propertyType = propertyTypeFromString(pType);
    if(propertyType == PlyPropertyTypes::kInvalid)
      throwRuntimeError("Invalid property type " + pType);

    // checking variable types for match with property types
    if((propertyType == PlyPropertyTypes::kX || propertyType == PlyPropertyTypes::kY || propertyType == PlyPropertyTypes::kZ
          || propertyType == PlyPropertyTypes::kX || propertyType == PlyPropertyTypes::kY || propertyType == PlyPropertyTypes::kZ)
        && variableType != PlyTypes::FLOAT32)
      throwRuntimeError("Property internal type and external type dont match.");

    if((propertyType == PlyPropertyTypes::kR || propertyType == PlyPropertyTypes::kG
          || propertyType == PlyPropertyTypes::kB || propertyType == PlyPropertyTypes::kA)
        && !(variableType == PlyTypes::INT8 || variableType == PlyTypes::UINT8))
      throwRuntimeError("Property internal type and external type dont match.");

    if((propertyType == PlyPropertyTypes::kFlags)
        && !(variableType == PlyTypes::INT32))
      throwRuntimeError("Property internal type and external type dont match.");

    if(propertyType == PlyPropertyTypes::kListInd
        && !(variableType == PlyTypes::INT16 || variableType == PlyTypes::INT32))
      throwRuntimeError("Property internal type and external type dont match.");

    if(propertyType == PlyPropertyTypes::kListTexCoords
        && variableType != PlyTypes::FLOAT32)
      throwRuntimeError("Property internal type and external type dont match.");
  }
};


/** \struct PlyElement
  * \brief Provides information about elements in PLY file
  *
  *
  */
struct PlyElement {
  PlyElementTypes             type;
  std::vector<PlyProperty>    properties;
  size_t                      count;
  size_t                      readCount;

  PlyElement() {
    count = 0;
    readCount = 0;
  }

  PlyElement(std::istringstream & is) {
    std::string token;
    is >> token >> count;
    if(token == "face"){
      DEBUG << "Face (" << count << ")";
      type = kFace;
    } else if (token == "vertex") {
      DEBUG << "Vertex (" << count << ")";
      type = kVertex;
    } else
      throwRuntimeError("Error: Invalid Element type");

    readCount = 0;
  }
};


/** \class PlyReader
  * \brief Enables the user to read Ply files to extract points and meshes
  *        from it.
  *
  * Supports Ascii and little endian binary formats of PLY.
  * Can read points and faces to memory or read them one at a time to save
  * memory.
  */
class PlyReader {
  private:
    std::ifstream             file; ///< stores the file stream
    bool                      isBinary; ///< reference for file type
    PlyElementTypes           curElement;

    /// stores information about the vertex element that stores points.
    PlyElement                pointElement;
    /// stores information about the face element.
    PlyElement                faceElement;
  public:
    // for non streaming
    std::vector<Point>        points; ///< vector accesible by user
    std::vector<Face>         faces; ///< vector accesible by user

    /** constructs class object
     *  requires valid ply file name else throws runtime exception
     */
    PlyReader (const std::string& filename) {
      file.open(filename, std::ifstream::in | std::ifstream::binary);

      if(!file.is_open())
        throwRuntimeError("Cant read ply file");

      readHeader();
    }

    ~PlyReader () {
      points.clear();
      faces.clear();
    }


    /** brief Reads all the contents of the file
     *
     *  Reads all elements and stores in ram
     */
    bool readFile();


    /*! brief Read one point at a time
     *
     *
     */
    bool readPoint(Point &v);

    inline size_t getPointsCount() const { return pointElement.count; }
    inline bool pointsEmpty() const {
      return pointElement.readCount < pointElement.count;
    }

  private:
    /// reads next face in stream
    bool readFace(Face &f);

    /// Reads PLY header to find the how to read the file
    void readHeader();

    /// readHeaderFormat - reads the file format of the ply file
    void readHeaderFormat(std::istringstream & is);

    /// readHeaderElement - reads the next element from stream
    void readHeaderElement(std::istringstream & is);

    /// readHeaderProperty - reads the next property from stream
    void readHeaderProperty(std::istringstream & is);


    template <typename T>
      inline T readProperty(const int& size) {
        if(isBinary)
          return readBinary<T>(size);
        else
          return readAscii<T>();
      }

    template <typename T>
      inline T readAscii () {
        T data;
        file >> data;
        return data;
      }

    template <typename T>
      inline T readBinary (const int& size) {
        T data;
        file.read(reinterpret_cast<char*>(&data), size);
        return data;
      }
}; // class PlyReader

/** \class PlyWriter
  * \brief Enables the user to export point clouds and meshes as Ply files.
  *
  * Supports Ascii and little endian binary formats of PLY.
  */
class PlyWriter {
  private:
    std::ofstream           file; ///< stores the file stream
    bool                    isBinary; ///< stores reference to file format

    /// stores information about the vertex element that stores points.
    PlyElement              pointElement;
    /// stores information about the face element.
    PlyElement              faceElement;

    size_t                  pointsCount;
    size_t                  faceCount;

  public:
    std::vector<Point>        points; ///< vector accesible by user
    std::vector<Face>         faces; ///< vector accesible by user

    /// constructs class object requires valid filename
    PlyWriter (const std::string& filename, bool binary = true)
      : isBinary(binary), pointsCount(0), faceCount(0) {
      file.open(filename, std::ofstream::out | std::ofstream::binary);

      if(!file.is_open())
        throwRuntimeError("Cant open file to write");
    }

    ~PlyWriter() {
      points.clear();
      faces.clear();
    }

    /// Adds properties for vertex element
    void addVertexElement ( bool _3DPoints = true, bool normals = false,
                            bool colors = false, bool flags = false);

    /// Adds properties for Face element
    void addFaceElement ( bool listVertIndices = true, bool texCoords = false,
                          bool colors = false, bool flags = false);

    /// Writes data available to Ply File
    void writeToFile() {
      writeHeader();
      writePoints(points);
      writeFaces();
      file.close();
      // LOG << points.size() << " points and "
      //    << faces.size() << " faces written to PLY file.";
    }

    void closeFile() {file.close(); }

    void setPointsCount(const size_t pc) { pointsCount = pc; }

    /// Writes the points to file stream
    void writePoints(const std::vector<Point> & points);

    /// Writes the PLY header to file
    void writeHeader();
   /// Write a single point to file stream
    void writePoint(const Point & p);
    /// Writes the faces to file stream
    void writeFaces();
  private:

    template <typename T>
      inline void writeProperty(T x, const int& size) {
        if(isBinary)
          writeBinary<T>(x, size);
        else
          writeAscii<T>(x, size);
      }

    template <typename T>
      inline void writeAscii (T x, const int& size) {
        file << x << " ";
      }

    template <typename T>
      inline void writeBinary (T x,const int& size) {
        file.write(reinterpret_cast<char*>(&x), size);
      }

}; // class PlyWriter
#endif // _PLYIO_H_
