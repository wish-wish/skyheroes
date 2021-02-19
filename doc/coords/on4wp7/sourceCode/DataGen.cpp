#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <vector>

using namespace std;

struct desc
{
	int x, y, z;
	int offset;
	int length;
	char *buff;
} dummy;

int FileSize(FILE *fp)
{
	long save, ret;
	save = ftell(fp);
	fseek(fp, 0, SEEK_END);
	ret = ftell(fp);
	fseek(fp, save, SEEK_SET);
	return (int)ret;
}

int main()
{
	int count, offset;
	FILE *fp;
	vector<desc> v;
	static char line[1024];
	static char name[100];
	
	fp = fopen("@list.txt", "r");
	if (!fp) goto error;
	while (1)
	{
		fgets(line, 1024, fp);
		if (strlen(line) < 5) continue;
		if (feof(fp)) break;
		char *p = line;
		dummy.x = atoi(p);
		p = strchr(p, '-') + 1;
		dummy.y = atoi(p);
		p = strchr(p, '-') + 1;
		dummy.z = atoi(p);
		v.push_back(dummy);
	}
	fclose(fp);
	
	printf("Total: %d\n", (int)v.size());
	
	for (size_t i = 0; i < v.size(); ++i)
	{
		sprintf(name, "%d-%d-%d.png", v[i].x, v[i].y, v[i].z);
		fp = fopen(name, "rb");
		if (!fp) goto error;
		v[i].length = FileSize(fp);
		v[i].buff = new char[v[i].length];
		if (v[i].length != (int)fread(v[i].buff, 1, v[i].length, fp)) goto error;
		fclose(fp);
	}
	
	puts("Writing...");
	
	fp = fopen("GOMapData", "wb");
	if (!fp) goto error;
	count = (int)v.size();
	if (fwrite(&count, 1, 4, fp) != 4) goto error;
	offset = 4 + 20 * count;
	for (size_t i = 0; i < v.size(); ++i)
	{
		v[i].offset = offset;
		offset += v[i].length;
		if (fwrite(&v[i], 1, 20, fp) != 20) goto error;
	}
	for (size_t i = 0; i < v.size(); ++i)
	{
		if (v[i].length != (int)fwrite(v[i].buff, 1, v[i].length, fp)) goto error;
	}
	fclose(fp);
	return 0;
error:
	puts("Error");
	return 1;
}
