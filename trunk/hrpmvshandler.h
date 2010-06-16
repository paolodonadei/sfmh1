#ifndef HRPMVSHANDLER_H
#define HRPMVSHANDLER_H

#include <sstream>
#include <string>
#include <vector>
#include <typeinfo>
#include "HRImage.hpp"

using namespace std;

class HRpmvshandler
{
    public:
        /** Default constructor */
       HRpmvshandler(string tempFolder, string origFolder , HRImageSet* pimSet);
        /** Default destructor */
        virtual ~HRpmvshandler();
        /** Access tempFOLD
         * \return The current value of tempFOLD
         */
        string GettempFOLD() { return tempFOLD; }
        /** Set tempFOLD
         * \param val New value to set
         */
        void SettempFOLD(string val) { tempFOLD = val; }
        /** Access origFOLD
         * \return The current value of origFOLD
         */
        string GetorigFOLD() { return origFOLD; }
        /** Set origFOLD
         * \param val New value to set
         */
        void SetorigFOLD(string val) { origFOLD = val; }
  void SetimSet(HRImageSet* pSet) { mimSet = pSet; }

        void createPMVSFolder();
        void copyImagesToPMVSFolder();
        void copyProjectionsToPMVSFolder();

         void runPMVSLinux();
         void runPMVSWindows();
         void cleanUpPMVS();
            void runPMVS();
                 void writeOptionFile();
    protected:
    private:
        string tempFOLD; //!< Member variable "tempFOLD"
        string origFOLD; //!< Member variable "origFOLD"
         HRImageSet* mimSet;
};

#endif // HRPMVSHANDLER_H
