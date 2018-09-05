#include <iostream>
#include <stdio.h>
#include <fstream>
#include <vector>
#include <cmath>


class EmptyFragmentInfo{
public:
    EmptyFragmentInfo(size_t localCounter, double maxAbsSampleValue, size_t position)
        : localCounter_(localCounter)
        , maxAbsSampleValue_ (maxAbsSampleValue)
        , position_(position)
    { }
    size_t localCounter_ = 0;
    double maxAbsSampleValue_ = 0.0;
    size_t position_;
};


void printSeparator(){
    std::cout << "----------------------------------------------------------------" << std::endl;
}

void printProgramDescription(){
    std::cout << "this program is used to find empty PCM fragment." << std::endl;
}

int main(int argc, char *argv[]){
    printSeparator();
    printProgramDescription();

    if (argc != 2){
        std::cout << "usage: ./fef pcmFileName" << std::endl;
        return -1;
    }

    auto s_threshold = 0.00001;
    auto s_samples = 100;
    {
        std::cout << "normalized sample threshold: " << s_threshold << std::endl;
        std::cout << "minimal framgent size: " << s_samples << " samples " << std::endl;
    }


    auto fileName = argv[1];


    std::ifstream ifs(fileName, std::ios::binary); // S16LE, aka short

    if (ifs.is_open()){

        auto begin = ifs.tellg();
        ifs.seekg (0, std::ios::end);
        auto end = ifs.tellg();
        auto totalFileSize = end-begin;
        std::cout << fileName << " file size is: " << totalFileSize << " bytes.\n";
        ifs.seekg (0, std::ios::beg);
        auto fileSizeRemain = totalFileSize;

        std::vector<EmptyFragmentInfo> fragmentList;


        size_t localCounter = 0;
        double localMaxAbsDoubleValue = 0.0;

        for (auto readerBufferSize = 1<<10; fileSizeRemain>0; fileSizeRemain -= readerBufferSize){
            if (readerBufferSize > fileSizeRemain){
                readerBufferSize = fileSizeRemain;
            }
            std::vector<short> buffer(readerBufferSize/sizeof(short));
            ifs.read((char*)(buffer.data()), readerBufferSize);

            for (int index = 0; index < readerBufferSize/sizeof(short); index++){
                auto absDoubleValue = std::abs(buffer[index]/(double)(1<<15));
                if (absDoubleValue < s_threshold){
                    localCounter++;
                    if (absDoubleValue>localMaxAbsDoubleValue){
                        localMaxAbsDoubleValue = absDoubleValue;
                    }
                }
                else {
                    if (localCounter>s_samples){
                        fragmentList.push_back({localCounter, localMaxAbsDoubleValue, totalFileSize-fileSizeRemain+index*(sizeof(short))});
                    }
                    localCounter=0;
                    localMaxAbsDoubleValue = 0.0;
                }
            }
        }



        if (fragmentList.size()==0){
            std::cout << "no empty fragment found" << std::endl;
        }
        else {
            printf("found %zu empty fragment%s(print format: offset+length {maxAbsSampleValue}}):\n", fragmentList.size(), fragmentList.size()>1?"s":"");
            for (auto &fragment : fragmentList){
                printf("\t@0x%zx \t +0x%zx(%zd bytes) \t {%f}\n",
                       fragment.position_ ,
                       fragment.localCounter_,
                       fragment.localCounter_,
                       fragment.maxAbsSampleValue_);
            }
        }

        ifs.close();
    }
    else {
        std::cout << "could not open " << fileName << std::endl;
    }


    printSeparator();
    return 0;
}
