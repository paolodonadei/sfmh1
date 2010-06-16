#ifndef HRPMVSHANDLER_H
#define HRPMVSHANDLER_H


class HRpmvshandler
{
    public:
        /** Default constructor */
        HRpmvshandler(string tempFolder, string origFolder);
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
    protected:
    private:
        string tempFOLD; //!< Member variable "tempFOLD"
        string origFOLD; //!< Member variable "origFOLD"
};

#endif // HRPMVSHANDLER_H
