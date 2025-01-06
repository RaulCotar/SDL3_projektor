#include <stdlib.h>
#include <stdio.h>
#include "opengl_boilerplate.h"
#include <SDL3/SDL.h>
#include <SDL3/SDL_init.h>
#include <SDL3/SDL_video.h>
#include <SDL3/SDL_events.h>
#include <SDL3/SDL_keycode.h>
#include "types.h"

struct GlobState {
	SDL_Window *window;
	SDL_GLContext glContext;
	u64 frameCount;
} glob; /// Holds all the global state of the program.

SDL_AppResult HandleCmdlineArgs(int argc, char *argv[argc]) {
	return SDL_APP_CONTINUE;
}

SDL_AppResult InitSDL() {
	SDL_InitSubSystem(SDL_INIT_VIDEO);

	SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 3);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_FLAGS, SDL_GL_CONTEXT_DEBUG_FLAG);

	glob.window = SDL_CreateWindow(SDL_GetAppMetadataProperty(SDL_PROP_APP_METADATA_NAME_STRING), 640, 480, SDL_WINDOW_OPENGL | SDL_WINDOW_RESIZABLE);
	if (glob.window == NULL)
		goto L_InitSDL_printSdlErrAndRetFail;
	glob.glContext = SDL_GL_CreateContext(glob.window);
	if (glob.glContext == NULL)
		goto L_InitSDL_printSdlErrAndRetFail;
	return SDL_APP_CONTINUE;

	L_InitSDL_printSdlErrAndRetFail:
	printf("Last SDL error: %s\n", SDL_GetError());
	return SDL_APP_FAILURE;
}

SDL_AppResult HandleEventsQueue() {
	SDL_AppResult res = SDL_APP_CONTINUE;
	SDL_Event e;
	while(SDL_PollEvent(&e)) {
		switch (e.type) {
			case SDL_EVENT_QUIT:
				res = SDL_APP_SUCCESS;
				break;
			case SDL_EVENT_KEY_DOWN:
				switch (e.key.key) {
					case SDLK_ESCAPE:
						res = SDL_APP_SUCCESS;
						break;
				}
				break;
			default:
				// printf("Unhandled SDL event type %u occured.\n", e.type); // this also reports mouse movements if you don't catch them above
		}
	}
	return res;
}

SDL_AppResult MainLoop() {
	puts("Running main loop...");
	while (true) {
		if (HandleEventsQueue() != SDL_APP_CONTINUE)
			break;
		// TODO: actually render something
		SDL_GL_SwapWindow(glob.window);
		glob.frameCount++;
	}
	puts("Exited main loop.");
	return SDL_APP_SUCCESS;
}

int main(int argc, char *argv[argc]) {
	atexit(SDL_Quit);
	SDL_SetAppMetadata("Projektor", APP_VERSION, "com.raulotar.projektor");
	if (HandleCmdlineArgs(argc, argv) != SDL_APP_CONTINUE ||
		InitSDL() != SDL_APP_CONTINUE ||
		LoadOpenGL(stderr, (GLADloadproc)SDL_GL_GetProcAddress) != SDL_APP_CONTINUE) {
		SDL_Quit();
		return 1;
	}

	// There are at least 4 OpenGL extensions that provide a "SetSwapInterval" function. Let SDL figure that out instead.
	if (SDL_GL_SetSwapInterval(SDL_WINDOW_SURFACE_VSYNC_ADAPTIVE))
		printf("Adaptive VSync enabled.\n");
	else {
		printf("Unable to set adaptive VSync! SDL Error: %s. Trying fallback...", SDL_GetError());
		if (SDL_GL_SetSwapInterval(1))
			printf("VSync enabled (every vblank).\n");
		else
			printf("Unable to enable VSync! SDL Error: %s.", SDL_GetError());
	}

	return MainLoop() - 1;
}
