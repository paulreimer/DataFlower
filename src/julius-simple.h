/* include top Julius library header */
#ifdef __cplusplus
extern "C" 
{
#endif

#include <julius/juliuslib.h>

/** 
 * Callback to be called when start waiting speech input. 
 * 
 */
static void
status_recready(Recog *recog, void *dummy);

/** 
 * Callback to be called when speech input is triggered.
 * 
 */
static void
status_recstart(Recog *recog, void *dummy);

/** 
 * Sub function to output phoneme sequence.
 * 
 */
static void
put_hypo_phoneme(WORD_ID *seq, int n, WORD_INFO *winfo);

/** 
 * Callback to output final recognition result.
 * This function will be called just after recognition of an input ends
 * 
 */
static void
output_result(Recog *recog, void *dummy);


/**
 * Main function
 * 
 */
void
speech_to_text_setup();

#ifdef __cplusplus
}
#endif

