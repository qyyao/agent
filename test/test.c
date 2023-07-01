// Copyright (c) 2019, Winfield Chen and Lloyd T. Elliott.

#include <stdio.h>
#include <stdarg.h>
#include <stddef.h>
#include <setjmp.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <cmocka.h>
#include "util.h"
#include "errors.h"
#include "statistics.h"
#include "matrix.h"
#include "agent.h"

void assert_double_equal(const double a, const double b, const double tolerance) {
    assert_true(fabs(a - b) < tolerance);
}

static void test_phenotypes(void **state) {
  (void) state;
  assert_crash("agent --genotypes XXX --phenotypes YYY data/wrong");
  assert_crash("agent --genotypes data0A/test01.a1 --phenotypes data0A/wrong01.txt data0A/wrong");
  assert_crash("agent --genotypes data0A/test01.a1 --phenotypes data0A/wrong02.txt data0A/wrong");
  assert_crash("agent --genotypes data0A/test01.a1 --phenotypes data0A/wrong03.txt data0A/wrong");
  assert_nocrash("agent --genotypes data0A/corner01.a1 --phenotypes data0A/test01.txt data0A/corner");
  assert_nocrash("agent --genotypes data0A/test01.a1 --phenotypes data0A/test01.txt data0A/test01");
}

static void test_arguments(void **state) {
  (void) state;

  assert_nocrash("agent --version");
  assert_crash("agent --version --version");
  assert_crash("agent -f");
  assert_crash("agent --genotypes xxx");
  assert_crash("agent --phenotypes xxx");
  assert_crash("agent --genotypes xxx yyy");
}


static void test_regression(void **state) {
  (void) state;

  // test01 
  {
    int D = 3;
    int N = 4;

    // t_matrix g = load_string(
    //   "V1 V2 V3 V4\n"
    //   "45 35 65 33\n"
    // );

    t_matrix g = load_string(
      "V1\n"
      "45\n"
      "35\n"
      "65\n"
      "33\n"
    );
    
    t_matrix y = load_string(
      "V1 V2 V3\n"
      "10 20 30\n"
      "40 50 60\n"
      "30 30 30\n"
      "20 25 32\n"
    );

    t_matrix denom = load_string(
      "a b c\n"
      "4 4 4\n"
    );

    for (int j = 0; j < D; j++) {
      double mu = 0.0;
      for (int i = 0; i < N; i++) {
        mu += get(y, i, j);
      }

      mu = mu / N;

      for (int i = 0; i < N; i++) {
        put(y, get(y, i, j) - mu, i, j);
      }
    }
    
    t_matrix obs = load_string(
      "a b c\n"
      "1 1 1\n"
      "1 1 1\n"
      "1 1 1\n"
      "1 1 1\n"
    );
    
    t_matrix beta = create(1, D);
    t_matrix se = create(1, D);
    t_matrix tstat = create(1, D);
    t_matrix pval = create(1, D);

    //MISSING PARAMETERS:
    t_matrix yt = create(D, N); 

    for (int i = 0; i < N; i++) {
      for (int j = 0; j < D; j++) {
        put(yt, get(y, i, j), j, i);
      }
    }

    int M = 4;

    t_matrix b1 = create(1, N); 
    t_matrix w1 = create(D, 1); 
    t_matrix w2 = create(0, 0); 

    regression(g, y, yt, obs, denom, beta, se, tstat, pval, b1, w1, w2);

    assert_double_equal(get(beta, 0, 0), 0.0155521, 1e-6);
    assert_double_equal(get(se, 0, 0), 0.5090393, 1e-6);
    assert_double_equal(get(pval, 0, 0), 0.009862875, 1e-8);
    assert_double_equal(get(beta, 0, 1), -0.2138414, 1e-6);
    assert_double_equal(get(se, 0, 1), 0.5036656, 1e-6);
    assert_double_equal(get(pval, 0, 1), 0.1550516, 1e-6);
    assert_double_equal(get(beta, 0, 2), -0.4790047, 1e-6);
    assert_double_equal(get(se, 0, 2), 0.509356, 1e-5);
    assert_double_equal(get(pval, 0, 2), 0.3805075, 1e-6);
  }
}

int main(void) {
  const struct CMUnitTest tests[] = {
    cmocka_unit_test(test_regression),
    cmocka_unit_test(test_arguments), 
    cmocka_unit_test(test_phenotypes)
  };

  return cmocka_run_group_tests(tests, NULL, NULL);
}

