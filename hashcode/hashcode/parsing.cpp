#include <iostream>
#include <fstream>
#include <sstream>
#include <array>

#include "Satellite.hpp"
#include "Collection.hpp"
#include "Simulation.hpp"
#include "TimeRange.hpp"
#include "utils.hpp"

/**
 * Different states of the reading of input file
 */
enum class ReadState {
	Collection,
	CollectionsNumber,
	NumberOfTurns,
	Satellites,
	SatellitesNumber,
	Photograph,
	TimeRange,
};

void Simulation::parseInput(const char* input_file, bool logging) {

	Simulation* simulation = this;

	LogMachine log("parsing", logging);

	log("Start");

	int cptSatellites;	// compteur d�croissant de satellites
	int cptCollections; // compteur d�croissant de collections
	int cptPhotos;		// compteur d�croissant de photos
	int cptTimeRanges;	// compteur d�croissant des fen�tres de temps

	std::ifstream input(input_file); // on cr�e un buffer de stream

	if (input.fail() || input.bad()) {
		throw ReadException(input_file);
	}

	std::string line; // ligne actuelle
	ReadState t = ReadState::NumberOfTurns; // �tat de l'automate de lecture

	while (std::getline(input, line))
	{
		int cpt = 0; // compteur d'�l�ment dans une ligne
		std::istringstream iss(line); // buffer de string

		std::string result;
		std::string result2;

		if (std::getline(iss, result, '\n'))
		{
			// deuxi�me buffer pour parser a l'int�rieur des lignes
			std::istringstream iss2(result);

			log("input : ", result);

			switch (t) {

				case ReadState::NumberOfTurns:
					log("nombre de tours", result);
					simulation->m_duration = std::stoi(result);
					t = ReadState::SatellitesNumber;
					break;

				case ReadState::SatellitesNumber:
					log("nombre de satellites", result);
					cptSatellites = std::stoi(result);
					simulation->m_number_of_satellites = cptSatellites;
					t = ReadState::Satellites;
					break;

				case ReadState::Satellites:
				{
					SatelliteLine satelliteLine;
					// lecture de la ligne � d�couper selon les espaces
					while (std::getline(iss2, result2, ' ')) {
						// on remplit un tableau intermediaire
						satelliteLine.at(cpt) = result2;
						// on passe a l'arg suivant de la ligne
						cpt++;
					}
					cpt = 0;
					Satellite* s = new Satellite(
						simulation->getNumberSatellites() - cptSatellites,
						satelliteLine
					);
					// on ajoute le satellite a la simulation
					this->m_satellites.push_back(s);

					log(*s);
					cptSatellites--; // next

					if (cptSatellites == 0) {
						// une fois qu'on a ajout� tous les satellites
						t = ReadState::CollectionsNumber;
					}
				}
					break;

				case ReadState::CollectionsNumber:

					log("nombre de collections", result);
					cptCollections = stoi(result);
					this->m_number_of_collections = cptCollections;
					t = ReadState::Collection;
					break;

				case ReadState::Collection:
				{
					CollectionLine collectionLine;

					// lecture de la ligne � d�couper selon les espaces
					while (std::getline(iss2, result2, ' ')) {
						// on remplit un tableau intermediaire
						collectionLine.at(cpt) = result2;
						// on passe a l'arg suivant de la ligne
						cpt++;
					}
					cpt = 0;

					Collection* c = new Collection(
						simulation->getNumberCollections() - cptCollections,
						collectionLine
					);
					// on ajoute la collection a la simulation
					this->m_collections.push_back(c);

					// compteur du nb de photos dans la collection
					cptPhotos	  = c->getNumberOfPhotographs();
					cptTimeRanges = c->getNumberOfTimeRanges();;
					cptCollections--; // next

					log(*c);

					// on passe aux photos de la collection
					t = ReadState::Photograph;

					break;
				}

				// FIXME : probl�me ici si on croise plusieurs fois la m�me
				// photo appartement � plusieurs collections, on cr�er plusieurs
				// objets
				case ReadState::Photograph:
				{
					PhotographLine PhotographLine;

					// lecture de la ligne � d�couper selon les espaces
					while (std::getline(iss2, result2, ' ')) {
						// on remplit un tableau intermediaire
						PhotographLine.at(cpt) = result2;
						cpt++; // on passe a l'arg suivant de la ligne
					}
					cpt = 0;

					Photograph* p = new Photograph(PhotographLine);
					log(*p);

					unsigned short collectionIndex =
						this->m_number_of_collections - cptCollections - 1;

					this->m_collections.at(collectionIndex)->add_photograph(p);
					// on ajoute la photo a la collection correspondante
					log("added to collection", collectionIndex);

					cptPhotos--;

					if (cptPhotos == 0) {
						// une fois qu'on a ajout� toutes les photos
						t = ReadState::TimeRange;
					}
				}
					break;

				case ReadState::TimeRange:
				{
					TimeRangeLine timeRangeLine;

					// lecture de la ligne � d�couper selon les espaces
					while (std::getline(iss2, result2, ' ')) {
						//std::cout << " Time ranges = :  " << result2 << std::endl;
						// on remplit un tableau intermediaire
						timeRangeLine.at(cpt) = result2;
						cpt++;
					}
					cpt = 0;

					TimeRange* time = new TimeRange(timeRangeLine);
					log(*time);

					unsigned short collectionIndex =
						this->m_number_of_collections - cptCollections - 1;

					// on ajoute la timerange a la collection correspondante
					this->m_collections.at(collectionIndex)
						->add_timeRange(time);
					log("added to collection", collectionIndex);

					cptTimeRanges--;

					if (cptTimeRanges == 0 && cptCollections == 0) {
						log("End");
						return;
					} else if (cptTimeRanges == 0) {
						// une fois qu'on a ajout� tous les Time Range,
						// prochaine collection
						t = ReadState::Collection;
					}
				}
					break;
			}
		}
	}

	input.close();
}
