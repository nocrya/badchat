const fs = require("fs");
const path = require("path");

const configPath = path.join(__dirname, "config.json");
const config = JSON.parse(fs.readFileSync(configPath, "utf8"));

function readEnv(key, fallback) {
    const value = process.env[key];
    if (value === undefined || value === null || value === "") {
        return fallback;
    }
    return value;
}

const email_user = readEnv("EMAIL_USER", config.email.user);
const email_pass = readEnv("EMAIL_PASS", config.email.pass);
const mysql_host = readEnv("MYSQL_HOST", config.mysql.host);
const mysql_port = Number(readEnv("MYSQL_PORT", config.mysql.port));
const redis_host = readEnv("REDIS_HOST", config.redis.host);
const redis_port = Number(readEnv("REDIS_PORT", config.redis.port));
const redis_passwd = readEnv("REDIS_PASSWORD", config.redis.passwd);
const grpc_port = Number(readEnv("VARIFY_GRPC_PORT", 50051));
const code_prefix = "code_";

module.exports = {
    email_pass,
    email_user,
    mysql_host,
    mysql_port,
    redis_host,
    redis_port,
    redis_passwd,
    grpc_port,
    code_prefix,
};