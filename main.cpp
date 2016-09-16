
#include "samplingfilter.h"
#include "fiberprocessing.h"

int main (int argc, char* argv[])
{
    GroupType::Pointer test;
    test = read_fiber("/work/dprince/AutoTract/Outputs/neo-0398-1-2year_42_DWI_QCed_VC_Upx2_DTI_stripped_embed.nrrd/Ped/3.PostProcess/Arc_L_FT_bundle_clean/Arc_L_FT_bundle_clean_processed.vtp");
    write_fiber("../ExampleOutput.vtp",test);

    return 0;
}
