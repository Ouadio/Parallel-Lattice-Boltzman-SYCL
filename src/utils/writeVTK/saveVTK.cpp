#include "saveVTK.h"

// for string tokenizer
#include <fstream>
#include <iostream>
#include <sstream>
#include <string>

static bool isBigEndian() {
  const int i = 1;
  return ((*(char *)&i) == 0);
}

// =======================================================
// =======================================================
template <typename T>
void saveVTK(const T *rho, const T *ux, const T *uy, const LBMParams<T> &params,
             bool outputVtkAscii, int iStep) {

  // some useful parameter alias
  const int nx = params.nx;
  const int ny = params.ny;

  const T dx = 1.0;
  const T dy = 1.0;

  const T xmin = 0.0;
  const T ymin = 0.0;

  const int nbPoints = nx * ny;

  // local variables
  std::string outputDir = "./";
  std::string outputPrefix = "lbm_data";

  // check scalar data type
  bool useDouble = false;

  if (sizeof(T) == sizeof(double)) {
    useDouble = true;
  }

  // write iStep in string stepNum
  std::ostringstream stepNum;
  stepNum.width(7);
  stepNum.fill('0');
  stepNum << iStep;

  // concatenate file prefix + file number + suffix
  std::string filename =
      outputDir + "/" + outputPrefix + "_" + stepNum.str() + ".vti";

  // open file
  std::fstream outFile;
  outFile.open(filename.c_str(), std::ios_base::out);

  // write header

  // if writing raw binary data (file does not respect XML standard)
  if (outputVtkAscii)
    outFile << "<?xml version=\"1.0\"?>\n";

  // write xml data header
  if (isBigEndian()) {
    outFile << "<VTKFile type=\"ImageData\" version=\"0.1\" "
               "byte_order=\"BigEndian\">\n";
  } else {
    outFile << "<VTKFile type=\"ImageData\" version=\"0.1\" "
               "byte_order=\"LittleEndian\">\n";
  }

  // write mesh extent
  outFile << "  <ImageData WholeExtent=\"" << 0 << " " << nx - 1 << " " << 0
          << " " << ny - 1 << " " << 0 << " " << 0 << "\" "
          << "Origin=\"" << xmin << " " << ymin << " " << 0.0 << "\" "
          << "Spacing=\"" << dx << " " << dy << " " << 0.0 << "\">\n";
  outFile << "  <Piece Extent=\"" << 0 << " " << nx - 1 << " " << 0 << " "
          << ny - 1 << " " << 0 << " " << 0 << " "
          << "\">\n";

  outFile << "    <CellData>\n";
  outFile << "    </CellData>\n";

  if (outputVtkAscii) {

    outFile << "    <PointData>\n";

    // write rho
    {

      // get variables string name
      const std::string varName = "rho";

      if (useDouble)
        outFile << "    <DataArray type=\"Float64\" ";
      else
        outFile << "    <DataArray type=\"Float32\" ";
      outFile << "Name=\"" << varName << "\" format=\"ascii\" >\n";

      for (int index = 0; index < nbPoints; ++index) {

        outFile << rho[index] << " ";
      }
      outFile << "\n    </DataArray>\n";
    } // end rho

    // write ux
    {

      // get variables string name
      const std::string varName = "ux";

      if (useDouble)
        outFile << "    <DataArray type=\"Float64\" ";
      else
        outFile << "    <DataArray type=\"Float32\" ";
      outFile << "Name=\"" << varName << "\" format=\"ascii\" >\n";

      for (int index = 0; index < nbPoints; ++index) {

        outFile << ux[index] << " ";
      }
      outFile << "\n    </DataArray>\n";
    } // end ux

    // write uy
    {

      // get variables string name
      const std::string varName = "uy";

      if (useDouble)
        outFile << "    <DataArray type=\"Float64\" ";
      else
        outFile << "    <DataArray type=\"Float32\" ";
      outFile << "Name=\"" << varName << "\" format=\"ascii\" >\n";

      for (int index = 0; index < nbPoints; ++index) {

        outFile << uy[index] << " ";
      }
      outFile << "\n    </DataArray>\n";
    } // end uy

    // write u = sqrt(ux*ux+uy*uy)
    {

      // get variables string name
      const std::string varName = "uNorm";

      if (useDouble)
        outFile << "    <DataArray type=\"Float64\" ";
      else
        outFile << "    <DataArray type=\"Float32\" ";
      outFile << "Name=\"" << varName << "\" format=\"ascii\" >\n";

      for (int index = 0; index < nbPoints; ++index) {

        outFile << sqrt(ux[index] * ux[index] + uy[index] * uy[index]) << " ";
      }
      outFile << "\n    </DataArray>\n";
    } // end ux

    outFile << "    </PointData>\n";

    // write footer
    outFile << "  </Piece>\n";
    outFile << "  </ImageData>\n";
    outFile << "</VTKFile>\n";

  } else { // write data in binary format

    outFile << "    <PointData>" << std::endl;

    // idVar is incremented for each scalar field
    int idVar = 0;

    // write rho
    {

      // get variables string name
      const std::string varName = "rho";

      if (useDouble) {
        outFile << "     <DataArray type=\"Float64\" Name=\"";
      } else {
        outFile << "     <DataArray type=\"Float32\" Name=\"";
      }
      outFile << varName << "\" format=\"appended\" offset=\""
              << idVar * nx * ny * sizeof(T) + idVar * sizeof(unsigned int)
              << "\" />" << std::endl;

      idVar++;
    } // end rho

    // write ux
    {

      // get variables string name
      const std::string varName = "ux";

      if (useDouble) {
        outFile << "     <DataArray type=\"Float64\" Name=\"";
      } else {
        outFile << "     <DataArray type=\"Float32\" Name=\"";
      }
      outFile << varName << "\" format=\"appended\" offset=\""
              << idVar * nx * ny * sizeof(T) + idVar * sizeof(unsigned int)
              << "\" />" << std::endl;

      idVar++;
    } // end ux

    // write uy
    {

      // get variables string name
      const std::string varName = "uy";

      if (useDouble) {
        outFile << "     <DataArray type=\"Float64\" Name=\"";
      } else {
        outFile << "     <DataArray type=\"Float32\" Name=\"";
      }
      outFile << varName << "\" format=\"appended\" offset=\""
              << idVar * nx * ny * sizeof(T) + idVar * sizeof(unsigned int)
              << "\" />" << std::endl;

      idVar++;
    } // end uy

    // write u=sqrt(ux*ux+uy*uy)
    {

      // get variables string name
      const std::string varName = "uNorm";

      if (useDouble) {
        outFile << "     <DataArray type=\"Float64\" Name=\"";
      } else {
        outFile << "     <DataArray type=\"Float32\" Name=\"";
      }
      outFile << varName << "\" format=\"appended\" offset=\""
              << idVar * nx * ny * sizeof(T) + idVar * sizeof(unsigned int)
              << "\" />" << std::endl;

      idVar++;
    } // end u=sqrt(ux*ux+uy*uy)

    outFile << "    </PointData>" << std::endl;
    outFile << "  </Piece>" << std::endl;
    outFile << "  </ImageData>" << std::endl;

    outFile << "  <AppendedData encoding=\"raw\">" << std::endl;

    // write the leading undescore
    outFile << "_";
    // then write heavy data (column major format)
    {
      unsigned int nbOfWords = nx * ny * sizeof(T);

      // write rho
      {
        outFile.write((char *)&nbOfWords, sizeof(unsigned int));
        for (int j = 0; j < ny; j++)
          for (int i = 0; i < nx; i++) {
            T tmp = rho[i + nx * j];
            outFile.write((char *)&tmp, sizeof(T));
          }
      } // end rho

      // write ux
      {
        outFile.write((char *)&nbOfWords, sizeof(unsigned int));
        for (int j = 0; j < ny; j++)
          for (int i = 0; i < nx; i++) {
            T tmp = ux[i + nx * j];
            outFile.write((char *)&tmp, sizeof(T));
          }
      } // end ux

      // write uy
      {
        outFile.write((char *)&nbOfWords, sizeof(unsigned int));
        for (int j = 0; j < ny; j++)
          for (int i = 0; i < nx; i++) {
            T tmp = uy[i + nx * j];
            outFile.write((char *)&tmp, sizeof(T));
          }
      } // end uy

      // write u=sqrt(ux*ux+uy*uy)
      {
        outFile.write((char *)&nbOfWords, sizeof(unsigned int));
        for (int j = 0; j < ny; j++)
          for (int i = 0; i < nx; i++) {
            T tmp = sqrt(ux[i + nx * j] * ux[i + nx * j] +
                         uy[i + nx * j] * uy[i + nx * j]);
            outFile.write((char *)&tmp, sizeof(T));
          }
      } // end u

    } // end heavy data

    outFile << "  </AppendedData>" << std::endl;
    outFile << "</VTKFile>" << std::endl;

  } // end ascii/binary heavy data write

  outFile.close();

} // saveVTK


template void saveVTK<float>(const float *rho, const float *ux, const float *uy, const LBMParams<float> &params,
             bool outputVtkAscii, int iStep);
template void saveVTK<double>(const double *rho, const double *ux, const double *uy, const LBMParams<double> &params,
             bool outputVtkAscii, int iStep);
