#include <S52.h>
#include <stdio.h>

#include <ogr_api.h>
#include <S57data.h>
#include <S52PL.h>
#include <S57ogr.h>
#include <deque>

int main(int argc, char *argv[])
{
    const char* filename = argv[1];    
    
    S52_init(1,1,1,1,NULL);


    S52_loadCell(filename, NULL);

    // OGRRegisterAll();
    // OGRDataSourceH hDS     = NULL;
    // OGRSFDriverH   hDriver = NULL;
    // hDS = OGROpen(filename, FALSE, &hDriver);
    
    // if (NULL == hDS) {
    //     PRINTF("WARNING: file loading failed (%s)\n", filename);
    //     return FALSE;
    // }

    // if (NULL == hDriver) {
    //     PRINTF("ERROR: OGR S57 driver not found\n");
    //     g_assert(0);
    //     return FALSE;
    // }

    // std::deque<S52_obj*> objs;

    // int nLayer = OGR_DS_GetLayerCount(hDS);
    // for (int iLayer=0; iLayer<nLayer; ++iLayer) {
    //     OGRLayerH       ogrlayer  = OGR_DS_GetLayer(hDS, iLayer);
    //     OGRFeatureDefnH defn      = OGR_L_GetLayerDefn(ogrlayer);
    //     const char     *layername = OGR_FD_GetName(defn);
    //     if (NULL==layername || NULL==ogrlayer) {
    //         PRINTF("ERROR: layername || ogrlayer || S52_loadLayer_cb is NULL\n");
    //         g_assert(0);
    //     }
    //     OGRFeatureH feature = NULL;
    //     while ( NULL != (feature = OGR_L_GetNextFeature((OGRLayerH)ogrlayer))) {
            
    //         S52_obj* obj  = (S52_obj*)S52_getObject(layername, feature);
    //         objs.push_back(obj);

    //         const char* str = NULL; //S52_PL_getCMDstr(obj);

    //         if (str != NULL) {

    //             // // -- obj priority -----------------------------
    //             // // get override prio state
    //             // int  prio = S52_PL_isPrioO(obj);
    //             // // get Display PRIority
    //             // S52_disPrio  disp_prio = S52_PL_getDPRI(obj);
    //             // // get RADAR Priority
    //             // S52_RadPrio rad = S52_PL_getRPRI(obj);
    //             // // get DISplay Category
    //             // S52_DisCat cat = S52_PL_getDISC(obj);
    //             // // get LUCM (view group) - not used {here for compliness
    //             // int viewingGroup = S52_PL_getLUCM(obj);
    //             // -- obj priority -----------------------------

    //             if (strcmp(str, "") != 0)
    //                 printf("%s \n", str);
                        
    //         }   
    //         OGR_F_Destroy(feature);
    //     }
    // }
    // //
    // for (auto obj : objs) {
    //     S52_processObject(obj);
    // }
    // OGRReleaseDataSource(hDS);
       
    call_app();
    
    //S52_doneCell(argv[1]);

    S52_done();

    return 0;
}