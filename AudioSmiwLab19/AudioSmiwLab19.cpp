#include <iostream>
#include <fstream>

class Oscillator {
private:
    int rate;
    int control;
    int mod;

public:
    Oscillator(int rate) {
        control = 1;
        mod = 0;
        rate = rate;
    }

    short int process(double xin) {
        short int yout;
        double m;

        m = (double)mod / rate;
        yout = m * xin;
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
    std::string filename = "123";
    transform(rawInput, rawOutput, filename);
    return 0;
}

void transform(std::ifstream& input, std::ofstream& output, std::string filename) {
    input.open(filename + ".raw", std::ios::binary);
    output.open(filename + "_tremolo.raw", std::ios::binary);

    char sampleInput[2];
    short int sample;                       // zmienna przechouwjaca 16 bitow pobranych z pliku RAW

    if (input.is_open()) {

        Oscillator oscillator(44100);                       // inicjalizacja oscylatora
              
        int i = 0;
        while (input.read(sampleInput, sizeof(short int))) // przetworzenie danych w petli
        {
            if (i == 44100)                 // sprawdzenie czy wczytano cala sekunde jesli tak to wyzerowanie           
                i = 0;
                                        
                                            // pobranie dolnych i gornych bitow 
            sample = sampleInput[0] << 8;  
            sample += sampleInput[1];

            // == transfromacja ==

            sample = oscillator.process(sample);                                // pobranie wartosci z oscylatora
                                            // dodanie wartosci spod oscylatora do aktualnie pobranych bitow
            oscillator.sweep();             // aktualizacja oscylatora
                                        

            // == koniec transformacji ==
            sampleInput[1] = sample;        // zapisanie przetworzonych danych do pliku
            sampleInput[0] = sample >> 8;

            output.write(sampleInput, sizeof(short int));
            i++;                            // inkrementacja licznika tablic przechowujacych sekund
        }
        input.close();
        output.close();
    }
}
