#include <iostream>
#include <fstream>

class Oscillator {
private:
    int rate; // cz�stotliwo�� z jak� chcemy wykona� efekt
    int control; // warto�� -1 lub 1 do inkrementacji lub dekrementacji mod
    int mod; // warto�� moduluj�ca amplitude, jej warto�� przechodzi mi�dzy warto�ciami 0 i rate
    double depth; // g��bia d�wi�ku - ma za zadanie wyciszy� d�wi�k do 70% wej�ciowego d�wi�ku, ze wzgl�du na szum i mo�liwo�� przekroczenia warto�ci maksymalnej
public:
    Oscillator(int rate) {
        this->control = 1;
        this->mod = 0;
        this->rate = rate;
        this->depth = 0.7;
    }

    short int process(int16_t xin) {
        int8_t lowbyte = xin;       // pobieramy dolny bajt d�wi�ku
        int8_t highbyte = xin >> 8; // pobieramy g�rny bajt d�wi�ku

        short int yout;
        double m;

        m = (double)mod * depth / rate; // obliczamy modulacj� amplitudy od 0 do 1 pomno�one razy depth, czyli wsp�czynnik z zakresu 0 - 1 maj�cy za zadanie przyciszy� d�wi�k
        lowbyte = lowbyte * m; // mno�ymy dolny bajt przez modulacj�
        highbyte = highbyte * m; // mno�ymy g�rny bajt przez modulacj�
        
        yout = highbyte << 8; // zapisujemy bajty do 16-bitowej zmiennej;
        yout += lowbyte;

        return yout;
    }

    void sweep() { // funkcja tworzy fal� tr�jk�tn� do modulacji amplitudy inkrementuj�c i dekrementuj�c liniowo mod
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
    std::string filename = "12";      // tutaj podajemy nazw� pliku wej�ciowego
    transform(rawInput, rawOutput, filename);
    return 0;
}

void transform(std::ifstream& input, std::ofstream& output, std::string filename) {
    input.open(filename + ".raw", std::ios::binary);
    output.open(filename + "_tremolo_4Hz.raw", std::ios::binary);  // tutaj dodajemy sufix do pliku wyj�ciowego

    char sampleInput[2];
    short int sample;                       // zmienna przechouwjaca 16 bitow pobranych z pliku RAW

    if (input.is_open()) {

        Oscillator oscillator(11025);                       // inicjalizacja oscylatora z zadan� cz�stotliwo�ci� ( to ona warunkuje cz�stotliwo�� efektu )
              
        while (input.read(sampleInput, sizeof(short int))) // przetworzenie danych w petli
        {
            sample = 0;
                                            // pobranie dolnych i g�rnych bajt�w 
            sample = sampleInput[0] << 8;  
            sample += sampleInput[1];

            // == transfromacja ==
            //sample = sample * 2;      // w celu zwi�kszenia amplitudy podstawowego pliku -> cele testowe, zbyt cichy d�wi�k mia� s�abo s�yszalny efekt
            sample = oscillator.process(sample);    // pobranie warto�ci z oscylatora
            
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
