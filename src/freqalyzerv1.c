/**
* \file freqalyzerv1.c
* \brief Programme principal
* \author Wenceslas MARIE SAINTE
* \version bêta
* \date 02 février 2015
*
* Programme d'analyse spectrale pour les signaux audio et radio
* (continuité du projet de L3 - niversité de Rennes 1 - France)
*/

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <SDL/SDL.h>
#include <SDL/SDL_image.h>
#include <fmod/fmod.h>
#include <fmod/fmod_errors.h>
#include "constantes.h"

/**
* \fn int main(int argc, char *argv[])
* \brief Fonction principale
*
* \param[in] argc
* \param[in] *argv[] Pointeur sur des arguments du programme
* \return EXIT_SUCCESS - Arrêt normal du programme
*/

int main(int argc, char *argv[])
{
	int att = 1, motion_state = 0, status = STOP;
	float volume;

	SDL_Event event;
	SDL_Surface *ecran = NULL, *fond = NULL, *play = NULL, *stop = NULL, *pause = NULL, *volumeup = NULL, *volumedown = NULL, *spectrum = NULL;
	SDL_Surface *iplay = NULL, *ipause = NULL, *istop = NULL, *ivolup = NULL, *ivoldo = NULL;

	SDL_Rect pos_fond, pos_spect, pos_play, pos_pause, pos_stop, pos_volumeup, pos_volumedown;

	printf("Démarrage de Freqalyzer\n");

	FMOD_SYSTEM *system;
	FMOD_SOUND *sound;
	FMOD_CHANNEL *channel=0;
	FMOD_RESULT result;
	FMOD_BOOL state;
	void *extradriverdata = 0;

	pos_fond.x = 0;
	pos_fond.y = 0;
	pos_spect.x = 100;
	pos_spect.y = 150;
	pos_play.x = 100;
	pos_play.y = 520;
	pos_pause.x = 100 + CONTROLLER_SIZE;
	pos_pause.y = 520;
	pos_stop.x = 100 + (CONTROLLER_SIZE * 2);
	pos_stop.y = 520;
	pos_volumeup.x = 100 + (CONTROLLER_SIZE * 3);
	pos_volumeup.y = 520;
	pos_volumedown.x = 100 + (CONTROLLER_SIZE * 4);
	pos_volumedown.y = 520;

	//Chargement en mémoire du système d'affichage SDL - Vidéo
	SDL_Init(SDL_INIT_VIDEO);

	if(SDL_Init(SDL_INIT_VIDEO) == -1)
	{
		fprintf(stderr, "Erreur d'initialisation de la SDL");
		exit(EXIT_FAILURE);
	}

	//Paramétrage et ouverture de la fenêtre
	ecran =  SDL_SetVideoMode(800, 600, 32, SDL_HWSURFACE | SDL_DOUBLEBUF);

	spectrum = SDL_CreateRGBSurface(SDL_HWSURFACE, 600, 350, 32, 0, 0, 0, 0);
	SDL_FillRect(spectrum, NULL, SDL_MapRGB(ecran->format, 0, 0, 0));

	//Titrage de la fenêtre
	SDL_WM_SetCaption("Freqalyzer", NULL);

	//Chargement des images
	fond = IMG_Load("../pictures/fond.jpg");
	W_GestionErreur(P_IMAGE, 0, fond, "fond.jpg");

	play = IMG_Load("../pictures/play.jpg");
	W_GestionErreur(P_IMAGE, 0, play, "play.jpg");

	pause = IMG_Load("../pictures/pause.jpg");
	W_GestionErreur(P_IMAGE, 0, pause, "pause.jpg");

	stop = IMG_Load("../pictures/stop.jpg");
	W_GestionErreur(P_IMAGE, 0, stop, "stop.jpg");

	volumeup = IMG_Load("../pictures/volume_up.jpg");
	W_GestionErreur(P_IMAGE, 0, volumeup, "volume_up.jpg");

	volumedown = IMG_Load("../pictures/volume_down.jpg");
	W_GestionErreur(P_IMAGE, 0, volumedown, "volume_down.jpg");

	iplay = IMG_Load("../pictures/iplay.jpg");
	W_GestionErreur(P_IMAGE, 0, iplay, "iplay.jpg");

	ipause = IMG_Load("../pictures/ipause.jpg");
	W_GestionErreur(P_IMAGE, 0, ipause, "ipause.jpg");

	istop = IMG_Load("../pictures/istop.jpg");
	W_GestionErreur(P_IMAGE, 0, istop, "istop.jpg");

	ivolup = IMG_Load("../pictures/ivolume_up.jpg");
	W_GestionErreur(P_IMAGE, 0, ivolup, "ivolume_up.jpg");

	ivoldo = IMG_Load("../pictures/ivolume_down.jpg");
	W_GestionErreur(P_IMAGE, 0, ivoldo, "ivolume_down.jpg");

	//Allocation de mémoire à system
	result = FMOD_System_Create(&system);
	W_GestionErreur(P_CREASON, result, NULL, "");

	//Initialisation
	result = FMOD_System_Init(system, 32, FMOD_INIT_NORMAL, extradriverdata);
	W_GestionErreur(P_CHARGSON, result, NULL, "");

	result = FMOD_System_CreateSound(system, "../sounds/radio_sig.mp3", FMOD_2D | FMOD_CREATESTREAM, 0, &sound);

/*	//Chargement du son
	result = FMOD_System_CreateSound(system, argv[1], FMOD_2D | FMOD_CREATESTREAM, 0, &sound);
	if (result != 0)
	{
		printf("\nErreur de saisie, veuillez taper un chemin correct \n(ex : ./Freqalyser ../sounds/gameofthrones.mp3)\n");
		exit(EXIT_FAILURE);
	}*/

	while(att)
	{
		SDL_WaitEvent(&event);

		if (event.motion.x > 100 && event.motion.x < (100 + CONTROLLER_SIZE * 5) && event.motion.y > 520 && event.motion.y < (520 + CONTROLLER_SIZE))
		{
			motion_state = 1;
			W_event(ecran, iplay, ipause, istop, ivolup, ivoldo);
		}else
			motion_state = 0;

		switch(event.type)
		{
			case SDL_QUIT:
				att = 0;
				break;
			case SDL_MOUSEBUTTONUP:
				if (event.button.x > 100 && event.button.x < (100 + CONTROLLER_SIZE) && event.button.y > 520 && event.button.y < (520 + CONTROLLER_SIZE))
				{
					//jouer le son et mettre en pause le programme le temps de sa lecture
					result=FMOD_System_PlaySound(system, sound, 0, 0, &channel);
					W_GestionErreur(P_LECTURE, result, NULL, "");
					unsigned int length;
					FMOD_Sound_GetLength(sound, &length, FMOD_TIMEUNIT_MS);
					FMOD_Channel_GetVolume(channel, &volume);
					FMOD_Channel_GetVolume(channel, &volume);
					status = PLAY;
				}
				if (event.button.x > (100 + CONTROLLER_SIZE) && event.button.x < (100 + (CONTROLLER_SIZE * 2)) && event.button.y > 520 && event.button.y < (520 + CONTROLLER_SIZE))
				{
					FMOD_System_GetChannel(system, 512, &channel);
					FMOD_Channel_GetPaused(channel, &state);
					if(state)
					{
						FMOD_Channel_SetPaused(channel, 0);
						status = PLAY;
					}
					else
					{
						FMOD_Channel_SetPaused(channel, 1);
						status = PAUSE;
					}
				}
				if (event.button.x > (100 + CONTROLLER_SIZE * 2) && event.button.x < (100 + CONTROLLER_SIZE * 3) && event.button.y > 520 && event.button.y < (520 + CONTROLLER_SIZE))
				{
					FMOD_Channel_Stop(channel);
					status = STOP;
				}
				if (event.button.x > (100 + CONTROLLER_SIZE * 3) && event.button.x < (100 + CONTROLLER_SIZE * 4) && event.button.y > 520 && event.button.y < (520 + CONTROLLER_SIZE))
					result = W_GestionVolume(&volume, channel, UP);
				if (event.button.x > (100 + CONTROLLER_SIZE * 4) && event.button.x < (100 + CONTROLLER_SIZE * 5) && event.button.y > 520 && event.button.y < (520 + CONTROLLER_SIZE))
					result = W_GestionVolume(&volume, channel, DOWN);
				break;
			case SDL_KEYDOWN:
				switch (event.key.keysym.sym)
				{
					case SDLK_ESCAPE:
 						att = 0;
						break;
					case SDLK_SPACE:
						//jouer le son et mettre en pause le programme le temps de sa lecture
						result=FMOD_System_PlaySound(system, sound, 0, 0, &channel);
						unsigned int length;
						FMOD_Sound_GetLength(sound, &length, FMOD_TIMEUNIT_MS);
						FMOD_Channel_GetVolume(channel, &volume);
						FMOD_Channel_GetVolume(channel, &volume);
						status = PLAY;
						break;
					case SDLK_UP:
						result = W_GestionVolume(&volume, channel, UP);
						break;
					case SDLK_DOWN:
						result = W_GestionVolume(&volume, channel, DOWN);
						break;
					case SDLK_p:
						FMOD_System_GetChannel(system, 512, &channel);
						FMOD_Channel_GetPaused(channel, &state);
						if(state)
						{
							FMOD_Channel_SetPaused(channel, 0);
							status = PLAY;
						}
						else
						{
							FMOD_Channel_SetPaused(channel, 1);
							status = PAUSE;
						}
						break;
					case SDLK_s:
						FMOD_Channel_Stop(channel);
						status = STOP;
						break;
					default:
						break;
					}
			default:
				break;
		}

		//Affichage des surfaces
		SDL_BlitSurface(fond, NULL, ecran, &pos_fond);
		SDL_BlitSurface(spectrum, NULL, ecran, &pos_spect);
		switch (status)
		{
			case PLAY:
				SDL_BlitSurface(iplay, NULL, ecran, &pos_play);
				SDL_BlitSurface(pause, NULL, ecran, &pos_pause);
				SDL_BlitSurface(stop, NULL, ecran, &pos_stop);
				break;
			case PAUSE:
				SDL_BlitSurface(play, NULL, ecran, &pos_play);
				SDL_BlitSurface(ipause, NULL, ecran, &pos_pause);
				SDL_BlitSurface(stop, NULL, ecran, &pos_stop);
				break;
			case STOP:
				SDL_BlitSurface(play, NULL, ecran, &pos_play);
				SDL_BlitSurface(pause, NULL, ecran, &pos_pause);
				SDL_BlitSurface(istop, NULL, ecran, &pos_stop);
				break;
			default:
				break;
		}
		SDL_BlitSurface(volumeup, NULL, ecran, &pos_volumeup);
		SDL_BlitSurface(volumedown, NULL, ecran, &pos_volumedown);

		//Rafraîchissement de la fenêtre
		SDL_Flip(ecran);
		FMOD_System_Update(system);
	}

	//Fermeture et libération de l'objet system en mémoire
	FMOD_Sound_Release(sound);
	FMOD_System_Close(system);
	FMOD_System_Release(system);

	SDL_FreeSurface(spectrum);
	SDL_FreeSurface(fond);
	SDL_FreeSurface(play);
	SDL_FreeSurface(pause);
	SDL_FreeSurface(stop);
	SDL_FreeSurface(volumeup);
	SDL_FreeSurface(volumedown);
	SDL_FreeSurface(iplay);
	SDL_FreeSurface(ipause);
	SDL_FreeSurface(istop);
	SDL_FreeSurface(ivolup);
	SDL_FreeSurface(ivoldo);

	SDL_Quit();

	return EXIT_SUCCESS;
}
