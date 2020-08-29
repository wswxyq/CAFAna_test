#include "CAFAna/Analysis/CSVMaker.h"

#include <functional>
#include <iomanip>
#include <iostream>
#include <limits>
#include <stdexcept>
#include <vector>

#include "CAFAna/Core/Var.h"
#include "CAFAna/Core/MultiVar.h"

#include "StandardRecord/Proxy/SRProxy.h"

namespace ana
{

template<class T, class S>
bool printSeparatedValues(
  std::ostream         &output,
  const std::vector<T> &values,
  bool                 firstValuePrinted,
  S                    separator,
  std::function<void (std::ostream &, const T&)> printValue
)
{
  if (values.empty()) {
    return firstValuePrinted;
  }

  size_t startIdx = 0;

  if (! firstValuePrinted)
  {
    printValue(output, values[0]);
    startIdx = 1;
  }

  for (size_t i = startIdx; i < values.size(); i++)
  {
    output << separator;
    printValue(output, values[i]);
  }

  return true;
}

CSVMaker::CSVMaker(const std::string &input, const std::string &output)
  : SpectrumLoader(input), cut(nullptr), ofile(output)
{
  if (! ofile) {
    throw std::runtime_error("Failed to open output file");
  }

  ofile << std::setprecision(std::numeric_limits<double>::max_digits10);
}

void CSVMaker::setPrecision(int precision)
{
  ofile << std::setprecision(precision);
}

void CSVMaker::printHeader()
{
  bool firstValuePrinted = false;

  firstValuePrinted = printSeparatedValues<NamedVar, char>(
    ofile, vars, firstValuePrinted, ',',
    [this] (auto &output, auto &namedVar) { output << namedVar.first; }
  );

  printSeparatedValues<NamedMultiVar, char>(
    ofile, multiVars, firstValuePrinted, ',',
    [this] (auto &output, auto &namedMultiVar)
    { output << namedMultiVar.first; }
  );

  ofile << std::endl;
}

void CSVMaker::printRow(caf::SRProxy* sr)
{
  bool firstValuePrinted = false;

  firstValuePrinted = printSeparatedValues<NamedVar, char>(
    ofile, vars, firstValuePrinted, ',',
    [sr,this] (auto &output, auto &namedVar) { output << namedVar.second(sr); }
  );

  printSeparatedValues<NamedMultiVar, char>(
    ofile, multiVars, firstValuePrinted, ',',
    [sr,this] (auto &output, auto &namedMultiVar)
    {
      std::vector<double> values = namedMultiVar.second(sr);

      output << '"';

      printSeparatedValues<double, char>(
        output, values, false, ',',
        [this] (auto &output, auto &x) { output << x; }
      );

      output << '"';
    }
  );

  ofile << std::endl;
}

void CSVMaker::addVar(const std::string &name, const Var &var)
{
  vars.emplace_back(std::make_pair(name, var));
}

void CSVMaker::addMultiVar(const std::string &name, const MultiVar &multiVar)
{
  multiVars.emplace_back(std::make_pair(name, multiVar));
}

void CSVMaker::addVars(const std::vector<NamedVar> &vars)
{
  this->vars.insert(this->vars.end(), vars.begin(), vars.end());
}

void CSVMaker::addMultiVars(const std::vector<NamedMultiVar> &multiVars)
{
  this->multiVars.insert(
    this->multiVars.end(), multiVars.begin(), multiVars.end()
  );
}

void CSVMaker::setCut(const Cut &cut)
{
  this->cut = std::make_unique<Cut>(cut);
}

void CSVMaker::HandleRecord(caf::SRProxy* sr)
{
  if ((fSpillCut != nullptr) && !(*fSpillCut)(&sr->spill)) {
    return;
  }

  if ((cut != nullptr) && !(*cut)(sr)) {
    return;
  }

  printRow(sr);
}

void CSVMaker::Go()
{
  if (! Gone()) {
      printHeader();
  }

  SpectrumLoader::Go();

  ofile.close();
}

}
