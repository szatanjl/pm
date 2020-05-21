#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <argon2.h>

static int reduce(unsigned char x, unsigned char n)
{
	return ((unsigned int)x * (unsigned int)n) / 256u;
}

static void hash2pwd(char *pwd, const char *hash, size_t len)
{
	size_t types[] = { 2, 2, 1, 25 };
	size_t i, j, t;

	types[3] = len;

	for (i = 0; i < len; i++) {
		t = reduce(hash[i * 2], len - i);
		for (j = 0; t >= types[j]; j++) {
			t -= types[j];
		}
		types[j]--;

		switch (j) {
		case 0:
			pwd[i] = 'A' + reduce(hash[i * 2 + 1], 26);
			break;
		case 1:
			pwd[i] = '0' + reduce(hash[i * 2 + 1], 10);
			break;
		case 2:
			pwd[i] = '!';
			break;
		default:
			pwd[i] = 'a' + reduce(hash[i * 2 + 1], 26);
			break;
		}
	}
	pwd[len] = '\0';
}

static int genpwd(char *pwd, size_t pwd_len,
                  const char *user, size_t user_len,
                  const char *pass, size_t pass_len)
{
	static const int parallelism = 1;
	static const int m_cost = 32 * 1024;
	static const int t_cost = 25;
	static char salt[128 + 8], hash[128 * 2];

	if (pwd_len > 128 || pass_len > 128 || user_len > 128) {
		return 1;
	}

	memcpy(salt, "SALT", 4);
	memcpy(salt + 4, user, user_len);
	memcpy(salt + 4 + user_len, "SALT", 4);

	argon2i_hash_raw(t_cost, m_cost, parallelism,
	                 pass, pass_len,
	                 salt, user_len + 8,
	                 hash, pwd_len * 2);

	hash2pwd(pwd, hash, pwd_len);

	return 0;
}

int main(int argc, char *argv[])
{
	static char pwd[128 + 1], pass[128];
	size_t pwd_len = 25, pass_len = 0;
	int c;

	if (argc < 2) {
		return 2;
	}
	if (argc > 2) {
		pwd_len = atoi(argv[2]);
	}
	while ((c = getchar()) != EOF && c != '\n') {
		pass[pass_len] = c;
		pass_len++;
	}

	if (pass_len > 0) {
		if (genpwd(pwd, 25, "foo", 3, "bar", 3) != 0 ||
		    strcmp(pwd, "j8yeUxfvzwnxost!oGqhrxb9a") != 0) {
			printf("ERROR: wrong argon2 hash %s\n", pwd);
			return 1;
		}
		if (genpwd(pwd, 25, "foo", 3, pass, pass_len) != 0 ||
		    strcmp(pwd, "losOv!mavehfgk7q1vVknhzrr") != 0) {
			printf("ERROR: wrong passphrase\n");
			return 1;
		}
	}

	if (genpwd(pwd, pwd_len, argv[1], strlen(argv[1]), pass, pass_len) != 0) {
		printf("ERROR: could not generate password\n");
		return 1;
	}
	printf("%s\n", pwd);

	return 0;
}
