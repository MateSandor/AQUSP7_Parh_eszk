__kernel void cubic_equation(__global double * a, __global double * b, __global double * c, __global double * d, __global double * x1, __global double * x2, __global double * x3) {
    int idx = get_global_id(0);

    double p, q, r;

    p = b[idx] / a[idx];
    q = c[idx] / a[idx];
    r = d[idx] / a[idx];

    double Q = (pow(p, 2) - 3 * q) / 9;
    double R = (2 * pow(p, 3) - 9 * p * q + 27 * r) / 54;
    double D = pow(Q, 3) - pow(R, 2);

    if (D > 0) {
        double theta = acos(R / sqrt(pow(Q, 3)));
        x1[idx] = -2 * sqrt(Q) * cos(theta / 3) - p / 3;
        x2[idx] = -2 * sqrt(Q) * cos((theta + 2 * M_PI) / 3) - p / 3;
        x3[idx] = -2 * sqrt(Q) * cos((theta - 2 * M_PI) / 3) - p / 3;
    } else if (D == 0) {
        x1[idx] = -2 * cbrt(R) - p / 3;
        x2[idx] = cbrt(R) - p / 3;
    } else {
        double alpha = cbrt(sqrt(pow(R, 2) - pow(Q, 3)) + fabs(R));
        double beta = Q / alpha;
        if (R > 0) {
            x1[idx] = -p / 3 + alpha + beta;
            x2[idx] = -p / 3 - 0.5 * (alpha + beta);
            x3[idx] = x2[idx];
        } else {
            x1[idx] = -p / 3 + alpha + beta;
            x2[idx] = -p / 3 - alpha - beta;
            x3[idx] = x2[idx];
        }
    }
}