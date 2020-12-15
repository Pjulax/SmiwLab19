#include <iostream>
#include <fstream>

class Oscillator {
private:
    int rate; // czêstotliwoœæ z jak¹ chcemy wykonaæ efekt
    int control; // wartoœæ -1 lub 1 do inkrementacji lub dekrementacji mod
    int mod; // wartoœæ moduluj¹ca amplitude, jej wartoœæ przechodzi miêdzy wartoœciami 0 i rate
    double depth; // g³êbia dŸwiêku - ma za zadanie wyciszyæ dŸwiêk do 70% wejœciowego dŸwiêku, ze wzglêdu na szum i mo¿liwoœæ przekroczenia wartoœci maksymalnej
public:
    Oscillator(int rate) {
        this->control = 1;
        this->mod = 0;
        this->rate = rate;
        this->depth = 0.7;
    }

    short int process(int16_t xin) {
        int8_t lowbyte = xin;       // pobieramy dolny bajt dŸwiêku
        int8_t highbyte = xin >> 8; // pobieramy górny bajt dŸwiêku

        short int yout;
        double m;

        m = (double)mod * depth / rate; // obliczamy modulacjê amplitudy od 0 do 1 pomno¿one razy depth, czyli wspó³czynnik z zakresu 0 - 1 maj¹cy za zadanie przyciszyæ dŸwiêk
        lowbyte = lowbyte * m; // mno¿ymy dolny bajt przez modulacjê
        highbyte = highbyte * m; // mno¿ymy górny bajt przez modulacjê
        
        yout = highbyte << 8; // zapisujemy bajty do 16-bitowej zmiennej;
        yout += lowbyte;

        return yout;
    }

    void sweep() { // funkcja tworzy falê trójk¹tn¹ do modulacji amplitudy inkrementuj¹c i dekrementuj¹c liniowo mod
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
    std::string filename = "12";      // tutaj podajemy nazwê pliku wejœciowego
    transform(rawInput, rawOutput, filename);
    return 0;
}

void transform(std::ifstream& input, std::ofstream& output, std::string filename) {
    input.open(filename + ".raw", std::ios::binary);
    output.open(filename + "_tremolo_4Hz.raw", std::ios::binary);  // tutaj dodajemy sufix do pliku wyjœciowego

    char sampleInput[2];
    short int sample;                       // zmienna przechouwjaca 16 bitow pobranych z pliku RAW

    if (input.is_open()) {

        Oscillator oscillator(11025);                       // inicjalizacja oscylatora z zadan¹ czêstotliwoœci¹ ( to ona warunkuje czêstotliwoœæ efektu )
              
        while (input.read(sampleInput, sizeof(short int))) // przetworzenie danych w petli
        {
            sample = 0;
                                            // pobranie dolnych i górnych bajtów 
            sample = sampleInput[0] << 8;  
            sample += sampleInput[1];

            // == transfromacja ==
            //sample = sample * 2;      // w celu zwiêkszenia amplitudy podstawowego pliku -> cele testowe, zbyt cichy dŸwiêk mia³ s³abo s³yszalny efekt
            sample = oscillator.process(sample);    // pobranie wartoœci z oscylatora
            
            oscillator.sweep();             // aktualizacja modulacji oscylatora
            
            // == koniec transformacji ==
            sampleInput[1] = sample;        // zapisanie przetworzonych danych do pliku
            sampleInput[0] = sample >> 8;

            output.write(sampleInput, sizeof(short int));
        }
        input.close();
        output.close();
    }
}
