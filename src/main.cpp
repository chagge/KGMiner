#include "opts.h"
#include "rdf_parser.h"

#include <iostream>
#include <fstream>
#include <boost/foreach.hpp>

using namespace std;

void write_map(boost::unordered_map<std::string, unsigned int> &map, std::string file_path, std::string sep = ",") {
  std::fstream fs(file_path, std::fstream::out);
  for (boost::unordered_map<std::string, unsigned int>::const_iterator it = map.cbegin();
       it != map.cend(); it++) {
    fs << it->second << sep << it->first << std::endl;
  }
  fs.close();
}

void write_vector(std::vector<std::string> &vec, std::string file_path) {
  std::fstream fs(file_path, std::fstream::out);
  for (std::vector<std::string>::const_iterator it = vec.cbegin();
       it != vec.cend(); it++) {
    fs << *it << std::endl;
  }
}

int main(int argc, const char* argv[]) {

  opts options;

  if (!options.parse(argc, argv)) {
    return -1;
  }

  unsigned int resource_id = 1, ontology_id = 1;
  boost::unordered_map<std::string, unsigned int> resource_map = boost::unordered_map<std::string, unsigned int>(); // contains all elements under dbpedia.org/resource/
  boost::unordered_map<std::string, unsigned int> ontology_map = boost::unordered_map<std::string, unsigned int>(); // all others
  std::vector<std::string> output_edges;

  try{
    for(size_t i = 0; i < options.getInput_files().size(); i++) {
      rdf_parser rdfParser(options.getInput_files().at(i));
      rdfParser.read_triplets(resource_map, ontology_map, resource_id, ontology_id, output_edges);
      std::cout << options.getInput_files().at(i) << " processed.\n";
    }

  }catch (std::invalid_argument error) {
    std::cout << error.what();
    return -1;
  }

  std::cout << "writing " << options.getOutput_prefix() << ".nodes ...\n";
  write_map(resource_map, options.getOutput_prefix() + ".nodes");

  std::cout << "writing " << options.getOutput_prefix() << ".edgetypes ...\n";
  write_map(ontology_map, options.getOutput_prefix() + ".edgetypes");

  // Deduplicate edges
  std::cout << "dedpulicating edges, this may take a while ...\n";
  sort(output_edges.begin(), output_edges.end());
  output_edges.erase(unique(output_edges.begin(), output_edges.end()), output_edges.end());

  std::cout << "writing " << options.getOutput_prefix() << ".edgelist ...\n";
  write_vector(output_edges, options.getOutput_prefix() + ".edgelist");

  std::cout << "Job done!\nStatistics:\n"
  << "\t#Nodes: " << resource_id << std::endl
  << "\t#EdgeTypes: " << ontology_id << std::endl
  << "\t#Edges: " << output_edges.size() << std::endl;

  return 0;
}