#include <iostream>
#include <fstream>

class Oscillator {
private:
    int rate;
    int control;
    int mod;

public:
    Oscillator(int rate) {
        this->control = 1;
        this->mod = 0;
        this->rate = rate;
    }

    short int process(int16_t xin) {
        int8_t lowbyte = xin;
        int8_t highbyte = xin >> 8;

        short int yout;
        double m;

        m = (double)mod *0.5/ rate;
        //std::cout << m << "\n";
        lowbyte = lowbyte * m;
        highbyte = highbyte * m;
        yout = highbyte << 8;
        yout += lowbyte;
        //std::cout << yout << "\n";
        return yout;
    }

    void sweep() {
        mod += control;

        if (mod > rate) {
            control = -1;
        }
        else if (!mod) {
            control = 1;
        }
    }
};


void transform(std::ifstream& input, std::ofstream& output, std::string filename);

int main()
{
    std::ifstream rawInput;
    std::ofstream rawOutput;
    std::string filename = "1234";
    transform(rawInput, rawOutput, filename);
    return 0;
}

void transform(std::ifstream& input, std::ofstream& output, std::string filename) {
    input.open(filename + ".raw", std::ios::binary);
    output.open(filename + "_tremolo.raw", std::ios::binary);

    char sampleInput[2];
    short int sample;                       // zmienna przechouwjaca 16 bitow pobranych z pliku RAW

    if (input.is_open()) {

        Oscillator oscillator(8820);                       // inicjalizacja oscylatora
              
        while (input.read(sampleInput, sizeof(short int))) // przetworzenie danych w petli
        {
            sample = 0;
                                            // pobranie dolnych i gornych bitow 
            sample = sampleInput[0] << 8;  
            sample += sampleInput[1];

            // == transfromacja ==

            sample = oscillator.process(sample);                                // pobranie wartosci z oscylatora
            //sampleInput[0] = oscillator.process(sampleInput[0]);                                // pobranie wartosci z oscylatora
                                            // dodanie wartosci spod oscylatora do aktualnie pobranych bitow
            oscillator.sweep();             // aktualizacja oscylatora
                                        

            // == koniec transformacji ==
            sampleInput[1] = sample;        // zapisanie przetworzonych danych do pliku
            sampleInput[0] = sample >> 8;

            output.write(sampleInput, sizeof(short int));
        }
        input.close();
        output.close();
    }
}
