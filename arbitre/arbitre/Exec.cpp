#include "Exec.hpp"

void Exec::listFile(const char* INPUT){
		/* D'après Nico c'est pas la meilleure solution
		 * Théoriquement c'est valide partout, mais
		 * Sur Windows ça bug
		 *
		 * Le problème c'est que tu prends en compte aussi les Directory
		 */
        DIR *pDIR;
        if( pDIR=opendir(INPUT) ){
                while(entry = readdir(pDIR)){
                        if( strcmp(entry->d_name, ".") != 0 && strcmp(entry->d_name, "..") != 0 )
                        cout << entry->d_name << "\n";
                }
                closedir(pDIR);
        }
}

void Exec::duration(){
	std::string str(INPUT);
    std::chrono::time_point<std::chrono::system_clock> start, end;
    start = std::chrono::system_clock::now();
    system("../bin/hashcode ../hashcode/hashcode/data/forever_alone.in ../hashcode/hashcode/data/forever_alone.out");
    end = std::chrono::system_clock::now();
 
    int elapsed_seconds = std::chrono::duration_cast<std::chrono::seconds>
                             (end-start).count();
    std::time_t end_time = std::chrono::system_clock::to_time_t(end);
 
    std::cout << "finished computation at " << std::ctime(&end_time)
              << "elapsed time: " << elapsed_seconds << "s\n";
}
