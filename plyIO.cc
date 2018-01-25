/** @cond DOXYGEN_EXCLUDE
 *             __________
 *            / ________ .
 *           / / ______ . .
 *          / / /      . . .
 *         / / /        . . .
 *         . . .        / / /
 *          . . .______/ / /
 *           . .________/ /
 *            .__________/
 *
 *              nomoko AG
 *
 *        Röschibachstrasse 24
 *           CH-8037 Zurich
 *            Switzerland
 *
 * @endcond
 * @file        plyIO.cpp
 * @brief       PLY file access.
 * @details     Provides classes to interact with ply files..
 * @author      Srivathsan Murali <srivathsan92@gmail.com>
 * @date        2016-10-12 12:29
 * @copyright   Nomoko AG
 * @remarks     In development
 */
/*==============================================================================
// Chuck Norris doesn't do Burn Down charts, he does Smack Down charts.
==============================================================================*/


#include "plyIO.h"

// ----------------------------------------------------------------------------
// PlyReader
// ----------------------------------------------------------------------------

/// Reads PLY header to find the how to read the file
void PlyReader::readHeader() {
  std::string line;
  while (std::getline(file, line))
  {
    std::istringstream ls(line);
    std::string token;
    ls >> token;
    if (token == "ply" || token == "PLY" || token == "") {
      continue;
    }
    else if (token == "comment")    continue;
    else if (token == "format")     readHeaderFormat(ls);
    else if (token == "element")    readHeaderElement(ls);
    else if (token == "property")   readHeaderProperty(ls);
    else if (token == "end_header") break;
    else
      throwRuntimeError("Error: Cant read header");
  }
  // LOG << "Read PLY header ("
  //     << pointElement.count << " points, "
  //     << faceElement.count << " face )";

} // readHeader

/// readHeaderFormat - reads the file format of the ply file
void PlyReader::readHeaderFormat (std::istringstream & is) {
  std::string s;
  (is >> s);
  isBinary = false;
  if (s == "binary_little_endian")
    isBinary = true;
  else if (s == "binary_big_endian")
    throwRuntimeError("big endian formats are not supported!");
  if (isBinary) DEBUG << "Format Binary";
  else DEBUG << "Format Ascii";
} // readHeaderFormat

/// readHeaderElement - reads the next element from stream
void PlyReader::readHeaderElement (std::istringstream & is) {
  PlyElement pe(is);

  if (pe.type == PlyElementTypes::kVertex) pointElement = pe;
  else faceElement = pe;

  curElement = pe.type;
}

/// readHeaderProperty - reads the next property from stream
void PlyReader::readHeaderProperty (std::istringstream & is) {
  PlyProperty pp (is);
  if (curElement == PlyElementTypes::kVertex)
    pointElement.properties.push_back(pp);
  else
    faceElement.properties.push_back(pp);
}

/// Reads all elements and stores in ram
bool PlyReader::readFile() {
  const size_t numPoints(pointElement.count);
  const size_t numFaces(faceElement.count);

  for(size_t i =0 ; i < numPoints; i++ ) {
    Point v;
    readPoint(v);
    points.push_back(v);
  }
  for(size_t i =0 ; i < numFaces; i++ ) {
    Face f;
    readFace(f);
    faces.push_back(f);
  }
  // LOG << "Read " << points.size() << " points and "
  //   << faces.size() << " faces.";
  return true;
}

/// Reads the next point in the stream
bool PlyReader::readPoint(Point &v) {
  if(pointElement.readCount == pointElement.count){
    DEBUG << "All points have been read.";
    return false;
  }

  for(const PlyProperty& prop : pointElement.properties) {
    const int size = TypeTable[prop.variableType].stride;
    switch (prop.propertyType) {
      case PlyPropertyTypes::kX:
        v.x = readProperty<float> (size);
        break;
      case PlyPropertyTypes::kY:
        v.y = readProperty<float> (size);
        break;
      case PlyPropertyTypes::kZ:
        v.z = readProperty<float> (size);
        break;
      case PlyPropertyTypes::kNX:
        v.nx = readProperty<float> (size);
        break;
      case PlyPropertyTypes::kNY:
        v.ny = readProperty<float> (size);
        break;
      case PlyPropertyTypes::kNZ:
        v.nz = readProperty<float> (size);
        break;
      case PlyPropertyTypes::kR:
        v.r = readProperty<unsigned int> (size);
        break;
      case PlyPropertyTypes::kG:
        v.g = readProperty<unsigned int> (size);
        break;
      case PlyPropertyTypes::kB:
        v.b = readProperty<unsigned int> (size);
        break;
      case PlyPropertyTypes::kA:
        v.a = readProperty<unsigned int> (size);
        break;
      case PlyPropertyTypes::kFlags:
        v.flags = readProperty<int> (size);
        break;
      default:
        DEBUG << "Property type : " << PropertyTypeTable[prop.propertyType]
              << " ( " << TypeTable[prop.variableType].str << ","
              << TypeTable[prop.variableType].stride << " ) ";
        throwRuntimeError("Not property type of vertex");
    }
  }
  pointElement.readCount++;
  return true;
}

/// Reads the next face in the stream
bool PlyReader::readFace(Face &f) {
  if(faceElement.readCount == faceElement.count){
    DEBUG << "Read all faces";
    return false;
  }

  for(const PlyProperty& prop : faceElement.properties) {
    const int size = TypeTable[prop.variableType].stride;
    int numList;
    switch (prop.propertyType) {
      case PlyPropertyTypes::kR:
        f.r = readProperty<unsigned int> (size);
        break;
      case PlyPropertyTypes::kG:
        f.g = readProperty<unsigned int> (size);
        break;
      case PlyPropertyTypes::kB:
        f.b = readProperty<unsigned int> (size);
        break;
      case PlyPropertyTypes::kA:
        f.a = readProperty<unsigned int> (size);
        break;
      case PlyPropertyTypes::kListInd:
        numList = readProperty<int> (TypeTable[prop.listSizeType].stride);
        f.ind.clear();
        for(int i =0; i< numList; i++)
          f.ind.push_back(readProperty<int> (size));
        break;
      case PlyPropertyTypes::kListTexCoords:
        numList = readProperty<int> (TypeTable[prop.listSizeType].stride);
        f.texCoords.clear();
        for(int i =0; i< numList; i++)
          f.texCoords.push_back(readProperty<float> (size));
        break;
      case PlyPropertyTypes::kFlags:
        f.flags = readProperty<int> (size);
        break;
      default:
        throwRuntimeError("Not property type of face");

    }
  }
  faceElement.readCount++;
  return true;
}

// ----------------------------------------------------------------------------
// PlyWriter
// ----------------------------------------------------------------------------

/// Writes the PLY header to file
void PlyWriter::writeHeader() {
  file  << "ply" << std::endl
        << (isBinary? "format binary_little_endian 1.0" :
            "format ascii 1.0") << std::endl
        << "comment plyIO generated" << std::endl;

  if (pointsCount == 0) pointsCount = points.size();
  if (faceCount == 0) faceCount = faces.size();

  if(pointElement.properties.size() > 0){
    file << "element vertex " << pointsCount << std::endl;
    for(const auto & prop : pointElement.properties) {
      file << "property ";
      if (prop.isList) {
        file << "list " << TypeTable[prop.listSizeType].str << " ";
      }
      file  << TypeTable[prop.variableType].str << " "
            << PropertyTypeTable[prop.propertyType] << std::endl;
    }
  }

  if(faceElement.properties.size() > 0) {
    file << "element face " << faceCount << std::endl;

    for(const auto & prop : faceElement.properties) {
      file << "property ";
      if (prop.isList) {
        file << "list " << TypeTable[prop.listSizeType].str << " ";
      }
      file  << TypeTable[prop.variableType].str << " "
        << PropertyTypeTable[prop.propertyType] << std::endl;
    }
  }
  file << "end_header" << std::endl;
}

/// Writes the points to file stream
void PlyWriter::writePoints(const std::vector<Point> & points_) {
  for(const auto & v : points_) {
    writePoint(v);
  }
}

void PlyWriter::writePoint(const Point & v) {
  for(const auto & prop : pointElement.properties) {
    const int size = TypeTable[prop.variableType].stride;
    switch (prop.propertyType) {
      case PlyPropertyTypes::kX:
        writeProperty<float>(v.x, size);
        break;
      case PlyPropertyTypes::kY:
        writeProperty<float>(v.y, size);
        break;
      case PlyPropertyTypes::kZ:
        writeProperty<float>(v.z, size);
        break;
      case PlyPropertyTypes::kNX:
        writeProperty<float>(v.nx, size);
        break;
      case PlyPropertyTypes::kNY:
        writeProperty<float>(v.ny, size);
        break;
      case PlyPropertyTypes::kNZ:
        writeProperty<float>(v.nz, size);
        break;
      case PlyPropertyTypes::kR:
        writeProperty<uint8_t>(v.r, size);
        break;
      case PlyPropertyTypes::kG:
        writeProperty<uint8_t>(v.g, size);
        break;
      case PlyPropertyTypes::kB:
        writeProperty<uint8_t>(v.b, size);
        break;
      case PlyPropertyTypes::kA:
        writeProperty<uint8_t>(v.a, size);
        break;
      case PlyPropertyTypes::kFlags:
        writeProperty<int>(v.flags, size);
        break;
      default:
        throwRuntimeError("Invalid property type");
    }
  }
  if(!isBinary) file << std::endl;
}
/// Writes the faces to file stream
void PlyWriter::writeFaces() {
  for(const auto & f : faces) {
    for(const auto & prop : faceElement.properties) {
      const int size = TypeTable[prop.variableType].stride;
      int listVarSize = 0;
      if(prop.isList)
        listVarSize = TypeTable[prop.listSizeType].stride;
      switch (prop.propertyType) {
        case PlyPropertyTypes::kR:
          writeProperty<uint8_t>(f.r, size);
          break;
        case PlyPropertyTypes::kG:
          writeProperty<uint8_t>(f.g, size);
          break;
        case PlyPropertyTypes::kB:
          writeProperty<uint8_t>(f.b, size);
          break;
        case PlyPropertyTypes::kA:
          writeProperty<uint8_t>(f.a, size);
          break;
        case PlyPropertyTypes::kListInd:
          writeProperty<uint8_t>(f.ind.size(), listVarSize);
          for(const auto & i : f.ind)
            writeProperty<int>(i, size);
          break;
        case PlyPropertyTypes::kListTexCoords:
          writeProperty<uint8_t>(f.ind.size(), listVarSize);
          for(const auto & i : f.ind)
            writeProperty<float>(i, size);
          break;
        case PlyPropertyTypes::kFlags:
          writeProperty<int>(f.flags, size);
          break;
        default:
          throwRuntimeError("Invalid property type");
      }
    }
    if(!isBinary) file << std::endl;
  }
}

/// Adds properties for vertex element
void PlyWriter::addVertexElement ( bool _3DPoints, bool normals,
                        bool colors, bool flags) {
  if(_3DPoints) {
    PlyProperty propertyX;
    propertyX.propertyType = PlyPropertyTypes::kX;
    propertyX.variableType = PlyTypes::FLOAT32;
    propertyX.isList       = false;
    pointElement.properties.push_back(propertyX);

    PlyProperty propertyY;
    propertyY.propertyType = PlyPropertyTypes::kY;
    propertyY.variableType = PlyTypes::FLOAT32;
    propertyY.isList       = false;
    pointElement.properties.push_back(propertyY);

    PlyProperty propertyZ;
    propertyZ.propertyType = PlyPropertyTypes::kZ;
    propertyZ.variableType = PlyTypes::FLOAT32;
    propertyZ.isList       = false;
    pointElement.properties.push_back(propertyZ);
  }

  if(normals) {
    PlyProperty propertyNX;
    propertyNX.propertyType = PlyPropertyTypes::kNX;
    propertyNX.variableType = PlyTypes::FLOAT32;
    propertyNX.isList       = false;
    pointElement.properties.push_back(propertyNX);

    PlyProperty propertyNY;
    propertyNY.propertyType = PlyPropertyTypes::kNY;
    propertyNY.variableType = PlyTypes::FLOAT32;
    propertyNY.isList       = false;
    pointElement.properties.push_back(propertyNY);

    PlyProperty propertyNZ;
    propertyNZ.propertyType = PlyPropertyTypes::kNZ;
    propertyNZ.variableType = PlyTypes::FLOAT32;
    propertyNZ.isList       = false;
    pointElement.properties.push_back(propertyNZ);
  }

  if(colors) {
    PlyProperty propertyR;
    propertyR.propertyType = PlyPropertyTypes::kR;
    propertyR.variableType = PlyTypes::UINT8;
    propertyR.isList       = false;
    pointElement.properties.push_back(propertyR);

    PlyProperty propertyG;
    propertyG.propertyType = PlyPropertyTypes::kG;
    propertyG.variableType = PlyTypes::UINT8;
    propertyG.isList       = false;
    pointElement.properties.push_back(propertyG);

    PlyProperty propertyB;
    propertyB.propertyType = PlyPropertyTypes::kB;
    propertyB.variableType = PlyTypes::UINT8;
    propertyB.isList       = false;
    pointElement.properties.push_back(propertyB);

    PlyProperty propertyA;
    propertyA.propertyType = PlyPropertyTypes::kA;
    propertyA.variableType = PlyTypes::UINT8;
    propertyA.isList       = false;
    pointElement.properties.push_back(propertyA);
  }

  if(flags) {
    PlyProperty propertyFlags;
    propertyFlags.propertyType = PlyPropertyTypes::kFlags;
    propertyFlags.variableType = PlyTypes::INT32;
    propertyFlags.isList       = false;
    pointElement.properties.push_back(propertyFlags);
  }
}

/// Adds properties for Face element
void PlyWriter:: addFaceElement ( bool listVertIndices,
        bool texCoords, bool colors, bool flags) {
  if(listVertIndices) {
    PlyProperty propertyInd;
    propertyInd.propertyType = PlyPropertyTypes::kListInd;
    propertyInd.variableType = PlyTypes::FLOAT32;
    propertyInd.isList       = true;
    faceElement.properties.push_back(propertyInd);
  }

  if(texCoords) {
    PlyProperty property;
    property.propertyType = PlyPropertyTypes::kListTexCoords;
    property.variableType = PlyTypes::UINT8;
    property.isList       = true;
    faceElement.properties.push_back(property);
  }

  if(colors) {
    PlyProperty propertyR;
    propertyR.propertyType = PlyPropertyTypes::kR;
    propertyR.variableType = PlyTypes::UINT8;
    propertyR.isList       = false;
    faceElement.properties.push_back(propertyR);

    PlyProperty propertyG;
    propertyG.propertyType = PlyPropertyTypes::kG;
    propertyG.variableType = PlyTypes::UINT8;
    propertyG.isList       = false;
    faceElement.properties.push_back(propertyG);

    PlyProperty propertyB;
    propertyB.propertyType = PlyPropertyTypes::kB;
    propertyB.variableType = PlyTypes::UINT8;
    propertyB.isList       = false;
    faceElement.properties.push_back(propertyB);

    PlyProperty propertyA;
    propertyA.propertyType = PlyPropertyTypes::kA;
    propertyA.variableType = PlyTypes::UINT8;
    propertyA.isList       = false;
    faceElement.properties.push_back(propertyA);
  }

  if(flags) {
    PlyProperty propertyFlags;
    propertyFlags.propertyType = PlyPropertyTypes::kFlags;
    propertyFlags.variableType = PlyTypes::INT32;
    propertyFlags.isList       = false;
    faceElement.properties.push_back(propertyFlags);
  }
}
