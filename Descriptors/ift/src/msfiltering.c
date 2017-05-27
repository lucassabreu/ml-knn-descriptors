#include "msfiltering.h"


void DestroyFilterbank( Filterbank ** pFilterbank ){

  register int s, k;

  if((*pFilterbank)!=NULL)
  {

    for( s = 0; s < (*pFilterbank)->scales; s++ ){
      for( k = 0; k < (*pFilterbank)->orientations; k++ ){
        DestroySpectrum( &((*pFilterbank)->m_pSpectralFilterBank[s*(*pFilterbank)->orientations+k]) );
      }
    }

    DestroyFeatures(&((*pFilterbank)->m_pFeatures));

    if((*pFilterbank)->m_pSpectralFilterBank != NULL) free((*pFilterbank)->m_pSpectralFilterBank);

    free((*pFilterbank));
    (*pFilterbank) = NULL;

  }
}


/** SP Filterbank creation **/

Filterbank* CreateSPFilterBank(int ncols, int nrows, int scales, int orientations, int nbands)
{
    if (!IsPowerOf2(ncols) || !IsPowerOf2(nrows))
    {
        Error("The Steerable Pyramid filterbank must have rows and cols power of 2","CreateSPFilterBank");
    }
    if (scales == 0)
    {
        Error("The scale must be > 0","CreateSPFilterBank");
    }
    if (orientations == 0)
    {
        Error("The orientations must be > 0","CreateSPFilterBank");
    }

    Filterbank* bank = (Filterbank*)calloc(1,sizeof(Filterbank));

    if (bank == NULL) Error(MSG1, "Filterbank");

    int size = scales*orientations;

    bank->m_pSpectralFilterBank = NULL;
    bank->m_pSpectralFilterBank = (Spectrum**)calloc(size,sizeof(Spectrum*));

    bank->m_pFeatures = NULL;
    bank->m_pFeatures = CreateFeatures(ncols,nrows,nbands*scales*orientations);

    bank->orientations = orientations;
    bank->scales = scales;

    int s,k;
    int width = ncols;
    int height = nrows;

    for (s = 0; s < scales; s++)
    {
        for (k = 0; k < orientations; k++)
        {
            Spectrum* filter = CreateSPBandPass(width,height,k+1,orientations, pow(2,s+1));
            Spectrum* shifted_filter = FFTShift(filter);

            bank->m_pSpectralFilterBank[s*orientations + k] = shifted_filter;
            DestroySpectrum(&filter);
        }
    }

    return bank;

}

void ApplySPFilterBank(DImage* img, Filterbank* bank, int startingnfeat)
{
    if (bank == NULL)
    {
        Error("Filterbank NULL", "ApplySPFilterBank");
    }
    if (bank->m_pSpectralFilterBank == NULL)
    {
        Error("Filterbank->m_pSpectralFilterBank NULL", "ApplySPFilterBank");
    }
    if (img->ncols != bank->m_pSpectralFilterBank[0]->ncols &&
            img->nrows != bank->m_pSpectralFilterBank[0]->nrows)
    {
        Error("DImage and bank are of different sizes","ApplySPFilterBank");
    }
    if (bank->m_pFeatures == NULL)
    {
        Error("m_pFeatures NULL","ApplySPFilterBank");
    }

    int s, k;
    int scales = bank->scales;
    int orientations = bank->orientations;
    int featindex = startingnfeat;
    Spectrum *smoothedDImg;
    Spectrum *imageSpec = DFFT2D(img);


    for (s = 0; s < scales; s++)
    {
        /// Applying low-pass filter
        Spectrum *lpSpec = CosLowPass(img->ncols, img->nrows, pow(2, s+1));

        smoothedDImg = ApplyFilter(imageSpec, lpSpec);

        DestroySpectrum(&lpSpec);

        for (k = 0; k < orientations; k++)
        {
            /// Applying band-pass filter
            Spectrum* filteredSpec = MultSpectrum(smoothedDImg, bank->m_pSpectralFilterBank[s*orientations+k]);
            DImage* filtered = DInvFFT2D(filteredSpec);

            /// Sets the rescaled image to the proper bank feature index
            SetFeature(bank->m_pFeatures,featindex++,filtered);

            DestroyDImage(&filtered);
            DestroySpectrum(&filteredSpec);
        }

        DestroySpectrum(&smoothedDImg);

    }

    DestroySpectrum(&imageSpec);

}

void ApplyMSLowPass(DImage* img, Filterbank* bank, int startingnfeat)
{
    if(img == NULL)
    {
        Error("DImage* NULL", "ApplyMSLowPass");
    }
    if (bank == NULL)
    {
        Error("Filterbank NULL", "ApplySPFilterBank");
    }

    int s;
    int featindex = startingnfeat;
    Spectrum*  smoothedDImg;
    Spectrum* dimageSpec = DFFT2D(img);
    Image* origimg = ConvertDImage2Image(img);

    for(s = 0; s < bank->scales; s++)
    {
        /// Applying low-pass filter
        /// The result of "pow(2.0,s+1)" guarantees that the filter radius will be
        /// reduced by half in each scale

        smoothedDImg = MultSpectrum(dimageSpec, bank->m_pSpectralFilterBank[s]);

        DImage* filtered = DInvFFT2D(smoothedDImg);

        /// Leveling image
        Image* ifiltered = ConvertDImage2Image(filtered);
        Image* result = Leveling(origimg,ifiltered);

        /// Sets the rescaled image to the proper feature index
        int j;
        for(j = 0; j < img->ncols*img->nrows; j++)
            bank->m_pFeatures->elem[j].feat[featindex] = (float)result->val[j];

        featindex++;

        /// Cleaning up
        DestroySpectrum(&smoothedDImg);
        DestroyDImage(&filtered);
        DestroyImage(&ifiltered);
        DestroyImage(&result);
    }
    DestroySpectrum(&dimageSpec);
    DestroyImage(&origimg);
}


Filterbank* CreateLPFilterBank(int ncols, int nrows, int nscales, int nbands)
{
    if (!IsPowerOf2(ncols) || !IsPowerOf2(nrows))
    {
        Error("The Low Pass filterbank must have rows and cols power of 2","CreateLPFilterBank");
    }
    if (nscales == 0)
    {
        Error("The scale must be > 0","CreateLPFilterBank");
    }

    Filterbank* bank = (Filterbank*)calloc(1,sizeof(Filterbank));

    if (bank == NULL) Error(MSG1, "Filterbank");

    bank->m_pSpectralFilterBank = NULL;
    bank->m_pSpectralFilterBank = (Spectrum**)calloc(nscales,sizeof(Spectrum*));

    bank->m_pFeatures = NULL;
    bank->m_pFeatures = CreateFeatures(ncols,nrows,nbands*nscales);

    bank->scales = nscales;
    bank->orientations = 1;

    int s;
    int width = ncols;
    int height = nrows;

    for (s = 0; s < nscales; s++)
    {
            Spectrum* filter = CosLowPass(width, height, pow(2,s+1));
            Spectrum* shifted_filter = FFTShift(filter);

            bank->m_pSpectralFilterBank[s] = shifted_filter;
            DestroySpectrum(&filter);
    }

    return bank;

}

