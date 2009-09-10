#pragma once

#include "SpeechSystem.h"

SpeechSystem::SpeechSystem() {
	verbose = SYSTEM_VERBOSE;
	if (verbose) printf("SpeechSystem::SpeechSystem()\n");
	jconf_filename = ofToDataPath("julian.jconf");

	lastSentDragger.content = &detected;
	lastSentDragger.enableMouseEvents();
	lastSentDragger.setPos(ofGetWidth()-200, ofGetHeight()-50);
	lastSentDragger.width = 200;
	lastSentDragger.height = 20;
	detected.sentences = "<<< none >>>";
}

SpeechSystem::~SpeechSystem() {
	if (verbose) printf("SpeechSystem::~SpeechSystem()\n");
	destroy();
}

void SpeechSystem::setup() {
	// ewwww
	jconf = j_config_load_file_new(const_cast<char*>(jconf_filename.c_str()));
	
	/* 2. create recognition instance according to the jconf */
	/* it loads models, setup final parameters, build lexicon
     and set up work area for recognition */
	recog = j_create_instance_from_jconf(jconf);
	if (recog == NULL) {
		fprintf(stderr, "Error in startup\n");
		return;
	}
	
	/*********************/
	/* Register callback */
	/*********************/
	/* register result callback functions */
	callback_add(recog, CALLBACK_EVENT_SPEECH_READY, recready, this);
	callback_add(recog, CALLBACK_EVENT_SPEECH_START, recstart, this);
	callback_add(recog, CALLBACK_RESULT, recdone, this);
	
	/**************************/
	/* Initialize audio input */
	/**************************/
	/* initialize audio input device */
	/* ad-in thread starts at this time for microphone */
	if (j_adin_init(recog) == FALSE) {    /* error */
		return;
	}

//#ifdef JULIUS_DEBUG
	/* output system information to log */
	j_recog_info(recog);
//#endif

	/***********************************/
	/* Open input stream and recognize */
	/***********************************/
	/* raw speech input (microphone etc.) */
	
	switch(j_open_stream(recog, NULL)) {
		case 0:			/* succeeded */
			break;
		case -1:      		/* error */
			fprintf(stderr, "error in input stream\n");
			return;
		case -2:			/* end of recognition process */
			fprintf(stderr, "failed to begin input stream\n");
			return;
	}
    
	startThread(true, false); // blocking, verbose
}

void SpeechSystem::threadedFunction() {
	/**********************/
	/* Recognization Loop */
	/**********************/
	/* enter main loop to recognize the input stream */
	/* finish after whole input has been processed and input reaches end */
	j_recognize_stream(recog);
	
	if (verbose) printf("SpeechSystem::threadedFunction()\n");
}

void SpeechSystem::destroy() {
	if (verbose) printf("SpeechSystem::destroy()\n");
	/* calling j_close_stream(recog) at any time will terminate
     recognition and exit j_recognize_stream() */
	j_close_stream(recog);
	
	stopThread();	
}

/** 
 * Callback to be called when start waiting speech input. 
 * 
 */
static void recready(Recog *recog, void *speech_sys) {
	if (recog->jconf->input.speech_input == SP_MIC || recog->jconf->input.speech_input == SP_NETAUDIO) {
		fprintf(stderr, "<<< please speak >>>\n");
	}
}

/** 
 * Callback to be called when speech input is triggered.
 * 
 */
static void recstart(Recog *recog, void *speech_sys) {	
	if (recog->jconf->input.speech_input == SP_MIC || recog->jconf->input.speech_input == SP_NETAUDIO) {
		fprintf(stderr, "\n<<< listening >>>\n\n");
	}
}

/** 
 * Sub function to output phoneme sequence.
 * 
 */
void put_hypo_phoneme(WORD_ID *seq, int n, WORD_INFO *winfo) {
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
static void recdone(Recog *recog, void *speech_sys) {
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
		if (r->result.status < 0) {
			/* no results obtained */
			debug_julius_result(r);
			/* continue to next process instance */
			continue;
		}
		
		/* output results for all the obtained sentences */
		winfo = r->lm->winfo;
		
		int it_end = min(r->result.sentnum, 1);
		for(n = 0; n < it_end; n++) { // for all sentences
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

			((SpeechSystem*)speech_sys)->detected.sentences = "";
			for(i=1;i<seqnum-1;i++)
				((SpeechSystem*)speech_sys)->detected.sentences += winfo->woutput[seq[i]];
		}
	}
	
	/* flush output buffer */
	fflush(stdout);
}

void debug_julius_result(RecogProcess *r) {
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
}
