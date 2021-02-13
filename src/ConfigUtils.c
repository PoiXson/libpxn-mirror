#include "ConfigUtils.h"

#include <stdlib.h>
#include <string.h>
#include <cjson/cJSON.h>

#include "FileUtils.h"




cJSON* load_config_file(char *file) {
	if (strlen(file) == 0)
		return NULL;
	if (get_file_type(file) != IS_FILE)
		return NULL;
	char *data;
	int len = load_text_file(file, &data);
	if (len < 0)
		return NULL;
	if (len == 0) {
		free(data);
		return NULL;
	}
	cJSON *json = cJSON_Parse(data);
	free(data);
	return json;
}

//TODO
//void config_free(Config *cfg) {
//	free(cfg);
//}
