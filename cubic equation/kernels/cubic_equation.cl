
typedef struct cubic_data{
	float a;
	float b;
	float c;
	float d;
	float x1;
	float x2;
	float x3;
}Cubic_data;

__kernel void cubic_equation(__global Cubic_data* buffer) {
    int idx = get_global_id(0);
    float M_PI = 3.14159265359;

    float p, q, r;

    p = buffer[idx].b / buffer[idx].a;
    q = buffer[idx].c / buffer[idx].a;
    r = buffer[idx].d / buffer[idx].a;

    float Q = (pow(p, 2) - 3 * q) / 9;
    float R = (2 * pow(p, 3) - 9 * p * q + 27 * r) / 54;
    float D = pow(Q, 3) - pow(R, 2);

    if (D > 0) {
        float theta = acos(R / sqrt(pow(Q, 3)));
        buffer[idx].x1 = -2 * sqrt(Q) * cos(theta / 3) - p / 3;
        buffer[idx].x2 = -2 * sqrt(Q) * cos((theta + 2 * M_PI) / 3) - p / 3;
        buffer[idx].x3 = -2 * sqrt(Q) * cos((theta - 2 * M_PI) / 3) - p / 3;
    } else if (D == 0) {
        buffer[idx].x1 = -2 * cbrt(R) - p / 3;
        buffer[idx].x2 = cbrt(R) - p / 3;
    } else {
        float alpha = cbrt(sqrt(pow(R, 2) - pow(Q, 3)) + fabs(R));
        float beta = Q / alpha;
        if (R > 0) {
            buffer[idx].x1 = -p / 3 + alpha + beta;
            buffer[idx].x2 = -p / 3 - 0.5 * (alpha + beta);
            buffer[idx].x3 = buffer[idx].x2;
        } else {
            buffer[idx].x1 = -p / 3 + alpha + beta;
            buffer[idx].x2 = -p / 3 - alpha - beta;
            buffer[idx].x3 = buffer[idx].x2;
        }
    }
}