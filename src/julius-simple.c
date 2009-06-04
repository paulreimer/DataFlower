/* include top Julius library header */
#ifdef __cplusplus
extern "C" 
{
#endif

/* include top Julius library header */
#include <julius/juliuslib.h>

/** 
 * Callback to be called when start waiting speech input. 
 * 
 */
static void
status_recready(Recog *recog, void *dummy) {
	if (recog->jconf->input.speech_input == SP_MIC || recog->jconf->input.speech_input == SP_NETAUDIO) {
		fprintf(stderr, "<<< please speak >>>\n");
	}
}

/** 
 * Callback to be called when speech input is triggered.
 * 
 */
static void
status_recstart(Recog *recog, void *dummy) {
	if (recog->jconf->input.speech_input == SP_MIC || recog->jconf->input.speech_input == SP_NETAUDIO) {
		fprintf(stderr, "\n<<< listening >>>\n\n");
	}
}

/** 
 * Sub function to output phoneme sequence.
 * 
 */
static void
put_hypo_phoneme(WORD_ID *seq, int n, WORD_INFO *winfo) {
	int i,j;
	WORD_ID w;
	static char buf[MAX_HMMNAME_LEN];

	if (seq != NULL) {
		for (i=0;i<n;i++) {
			if (i > 0) printf(" |");
			w = seq[i];
			for (j=0;j<winfo->wlen[w];j++) {
				center_name(winfo->wseq[w][j]->name, buf);
				printf(" %s", buf);
			}
		}
	}
	printf("\n");
}
/** 
 * Callback to output final recognition result.
 * This function will be called just after recognition of an input ends
 * 
 */
static void
output_result(Recog *recog, void *dummy) {
	int i, j;
	int len;
	WORD_INFO *winfo;
	WORD_ID *seq;
	int seqnum;
	int n;
	Sentence *s;
	RecogProcess *r;
	HMM_Logical *p;
	SentenceAlign *align;

	/*	all recognition results are stored at each recognition process
		instance
	*/
	for(r=recog->process_list;r;r=r->next) {

		/* skip the process if the process is not alive */
		if (! r->live) continue;

		/* result are in r->result.	See recog.h for details */

		/* check result status */
		if (r->result.status < 0) {			/* no results obtained */
			/* outout message according to the status code */
			switch(r->result.status) {
			case J_RESULT_STATUS_REJECT_POWER:
				printf("<input rejected by power>\n");
				break;
			case J_RESULT_STATUS_TERMINATE:
				printf("<input teminated by request>\n");
				break;
			case J_RESULT_STATUS_ONLY_SILENCE:
				printf("<input rejected by decoder (silence input result)>\n");
				break;
			case J_RESULT_STATUS_REJECT_GMM:
				printf("<input rejected by GMM>\n");
				break;
			case J_RESULT_STATUS_REJECT_SHORT:
				printf("<input rejected by short input>\n");
				break;
			case J_RESULT_STATUS_FAIL:
				printf("<search failed>\n");
				break;
			}
			/* continue to next process instance */
			continue;
		}

		/* output results for all the obtained sentences */
		winfo = r->lm->winfo;

		for(n = 0; n < r->result.sentnum; n++) { // for all sentences

			s = &(r->result.sent[n]);
			seq = s->word;
			seqnum = s->word_num;

			// output word sequence like Julius
			printf("sentence%d:", n+1);
			for(i=0;i<seqnum;i++) printf(" %s", winfo->woutput[seq[i]]);
			printf("\ncmscore%d:", n+1);
			for(i=0;i<seqnum;i++) printf(" %5.3f", s->confidence[i]);
			// AM and LM scores
			printf("\nscore%d: %f", n+1, s->score);
			put_hypo_phoneme(seq, seqnum, winfo);
		}
	}

	/* flush output buffer */
	fflush(stdout);
}

#ifdef __cplusplus
}
#endif
