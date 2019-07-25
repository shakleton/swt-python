#include "ccv_nnc.h"
#include "ccv_nnc_easy.h"
#include "ccv_nnc_internal.h"
#include "ccv_internal.h"
#include "_ccv_cnnp_model.h"

#pragma mark - Core Layers

static void _ccv_cnnp_add_build(ccv_cnnp_model_t* const self, ccv_nnc_symbolic_graph_t* const graph, const ccv_nnc_tensor_symbol_t* const inputs, const int input_size, ccv_nnc_tensor_symbol_t* const outputs, const int output_size)
{
	assert(output_size == 1);
	outputs[0] = ccv_nnc_tensor_symbol_new(graph, ccv_nnc_tensor_symbol_params(graph, inputs[0]), 0);
	ccv_nnc_graph_exec_symbol_new(graph, CMD_EWSUM_FORWARD(), inputs, input_size, outputs, output_size, 0);
}

static const ccv_cnnp_model_vtab_t ccv_cnnp_add_isa = {
	.build = _ccv_cnnp_add_build,
};

typedef struct {
	ccv_cnnp_model_t super;
	ccv_nnc_tensor_symbol_t output;
} ccv_cnnp_model_add_t;

ccv_cnnp_model_t* ccv_cnnp_add(void)
{
	ccv_cnnp_model_add_t* const model_add = (ccv_cnnp_model_add_t*)cccalloc(1, sizeof(ccv_cnnp_model_add_t));
	model_add->super.isa = &ccv_cnnp_add_isa;
	model_add->super.input_size = 1;
	model_add->super.outputs = &model_add->output;
	model_add->super.output_size = 1;
	return (ccv_cnnp_model_t*)model_add;
}

static void _ccv_cnnp_concat_build(ccv_cnnp_model_t* const self, ccv_nnc_symbolic_graph_t* const graph, const ccv_nnc_tensor_symbol_t* const inputs, const int input_size, ccv_nnc_tensor_symbol_t* const outputs, const int output_size)
{
	assert(output_size == 1);
	// TODO: Concatenate is not done yet.
}

static const ccv_cnnp_model_vtab_t ccv_cnnp_concat_isa = {
	.build = _ccv_cnnp_concat_build,
};

typedef struct {
	ccv_cnnp_model_t super;
	ccv_nnc_tensor_symbol_t output;
} ccv_cnnp_model_concat_t;

ccv_cnnp_model_t* ccv_cnnp_concat(void)
{
	ccv_cnnp_model_concat_t* const model_concat = (ccv_cnnp_model_concat_t*)cccalloc(1, sizeof(ccv_cnnp_model_concat_t));
	model_concat->super.isa = &ccv_cnnp_concat_isa;
	model_concat->super.input_size = 1;
	model_concat->super.outputs = &model_concat->output;
	model_concat->super.output_size = 1;
	return (ccv_cnnp_model_t*)model_concat;
}

typedef struct {
	ccv_cnnp_model_t super;
	ccv_nnc_tensor_symbol_t output;
	int dim[CCV_NNC_MAX_DIM_ALLOC];
} ccv_cnnp_model_reshape_t;

static void _ccv_cnnp_reshape_build(ccv_cnnp_model_t* const super, ccv_nnc_symbolic_graph_t* const graph, const ccv_nnc_tensor_symbol_t* const inputs, const int input_size, ccv_nnc_tensor_symbol_t* const outputs, const int output_size)
{
	assert(input_size == 1);
	assert(output_size == 1);
	ccv_cnnp_model_reshape_t* const self = (ccv_cnnp_model_reshape_t*)super;
	ccv_nnc_tensor_param_t params = ccv_nnc_tensor_symbol_params(graph, inputs[0]);
	memcpy(params.dim, self->dim, sizeof(params.dim));
	outputs[0] = ccv_nnc_tensor_symbol_alias_new(graph, inputs[0], DIM_ALLOC(), self->dim, params, 0);
}

static const ccv_cnnp_model_vtab_t ccv_cnnp_reshape_isa = {
	.build = _ccv_cnnp_reshape_build,
};

ccv_cnnp_model_t* ccv_cnnp_reshape(const int dim[CCV_NNC_MAX_DIM_ALLOC])
{
	ccv_cnnp_model_reshape_t* const model_reshape = (ccv_cnnp_model_reshape_t*)cccalloc(1, sizeof(ccv_cnnp_model_reshape_t));
	model_reshape->super.isa = &ccv_cnnp_reshape_isa;
	model_reshape->super.input_size = 1;
	model_reshape->super.outputs = &model_reshape->output;
	model_reshape->super.output_size = 1;
	memcpy(model_reshape->dim, dim, sizeof(model_reshape->dim));
	return (ccv_cnnp_model_t*)model_reshape;
}

typedef struct {
	ccv_cnnp_model_t super;
	ccv_nnc_tensor_symbol_t output;
} ccv_cnnp_model_flatten_t;

static void _ccv_cnnp_flatten_build(ccv_cnnp_model_t* const super, ccv_nnc_symbolic_graph_t* const graph, const ccv_nnc_tensor_symbol_t* const inputs, const int input_size, ccv_nnc_tensor_symbol_t* const outputs, const int output_size)
{
	assert(input_size == 1);
	assert(output_size == 1);
	const ccv_nnc_tensor_param_t params = ccv_nnc_tensor_symbol_params(graph, inputs[0]);
	ccv_nnc_tensor_param_t output_params = params;
	memset(output_params.dim, 0, sizeof(output_params.dim));
	output_params.dim[0] = ccv_nnc_tensor_get_n(params);
	assert(output_params.dim[0] > 0);
	output_params.dim[1] = ccv_nnc_tensor_count(params) / output_params.dim[0];
	outputs[0] = ccv_nnc_tensor_symbol_alias_new(graph, inputs[0], DIM_ALLOC(), output_params.dim, output_params, 0);
}

static const ccv_cnnp_model_vtab_t ccv_cnnp_flatten_isa = {
	.build = _ccv_cnnp_flatten_build,
};

ccv_cnnp_model_t* ccv_cnnp_flatten(void)
{
	ccv_cnnp_model_flatten_t* const model_flatten = (ccv_cnnp_model_flatten_t*)cccalloc(1, sizeof(ccv_cnnp_model_flatten_t));
	model_flatten->super.isa = &ccv_cnnp_flatten_isa;
	model_flatten->super.input_size = 1;
	model_flatten->super.outputs = &model_flatten->output;
	model_flatten->super.output_size = 1;
	return (ccv_cnnp_model_t*)model_flatten;
}

#pragma mark - Identity Layer

typedef struct {
	ccv_cnnp_model_t super;
	ccv_nnc_tensor_symbol_t output;
	ccv_nnc_tensor_symbol_t bias;
	ccv_nnc_tensor_symbol_t scale;
	struct {
		ccv_nnc_graph_exec_symbol_t exec;
		ccv_nnc_cmd_param_t params;
	} bnorm;
	ccv_array_t* zero_inits;
	ccv_array_t* retainables;
	ccv_cnnp_param_t params;
} ccv_cnnp_model_identity_t;

static void _ccv_cnnp_identity_build(ccv_cnnp_model_t* const super, ccv_nnc_symbolic_graph_t* const graph, const ccv_nnc_tensor_symbol_t* const inputs, const int input_size, ccv_nnc_tensor_symbol_t* const outputs, const int output_size)
{
	assert(input_size == 1);
	assert(output_size == 1);
	ccv_cnnp_model_identity_t* const self = (ccv_cnnp_model_identity_t*)super;
	const ccv_nnc_tensor_param_t params = ccv_nnc_tensor_symbol_params(graph, inputs[0]);
	ccv_nnc_tensor_symbol_t output = inputs[0];
	if (self->params.norm == CCV_CNNP_BATCH_NORM)
	{
		ccv_nnc_tensor_param_t bias_params = params;
		memset(bias_params.dim, 0, sizeof(bias_params.dim));
		// If the accuracy is not enough, bump it to 32-bit floating point.
		if (bias_params.datatype != CCV_32F || bias_params.datatype != CCV_64F)
			bias_params.datatype = CCV_32F;
		bias_params.dim[0] = ccv_nnc_tensor_get_c(params);
		output = ccv_nnc_tensor_symbol_new(graph, params, 0);
		// Both scale and bias are shared between if this model is reused.
		if (!self->scale.graph)
			self->scale = ccv_nnc_tensor_symbol_new(graph, bias_params, 0);
		if (!self->bias.graph)
			self->bias = ccv_nnc_tensor_symbol_new(graph, bias_params, 0);
		const ccv_nnc_tensor_symbol_t mean = ccv_nnc_tensor_symbol_new(graph, bias_params, 0);
		const ccv_nnc_tensor_symbol_t var = ccv_nnc_tensor_symbol_new(graph, bias_params, 0);
		// Otherwise, notice mean, var, saved_mean, saved_inv_std are not reused.
		if (!self->zero_inits)
			self->zero_inits = ccv_array_new(sizeof(ccv_nnc_tensor_symbol_t), 0, 0);
		ccv_array_push(self->zero_inits, &mean);
		ccv_array_push(self->zero_inits, &var);
		const ccv_nnc_tensor_symbol_t out_mean = ccv_nnc_tensor_symbol_new(graph, bias_params, 0);
		const ccv_nnc_tensor_symbol_t out_var = ccv_nnc_tensor_symbol_new(graph, bias_params, 0);
		if (!self->retainables)
			self->retainables = ccv_array_new(sizeof(ccv_nnc_tensor_symbol_t), 0, 0);
		ccv_array_push(self->retainables, &out_mean);
		ccv_array_push(self->retainables, &out_var);
		const ccv_nnc_tensor_symbol_t saved_mean = ccv_nnc_tensor_symbol_new(graph, bias_params, 0);
		const ccv_nnc_tensor_symbol_t saved_inv_std = ccv_nnc_tensor_symbol_new(graph, bias_params, 0);
		const int hw = ccv_nnc_tensor_hw(params, ccv_nnc_tensor_nd(params.dim));
		ccv_nnc_cmd_param_t batch_norm = {
			.bnorm = {
				.epsilon = 1e-4,
				.is_test = 0,
				.momentum = 0.9,
				.count = hw >= 0 ? CCV_NNC_MAX_DIM + 1 : 1,
			}
		};
		int i;
		batch_norm.bnorm.axis[0] = (params.format == CCV_TENSOR_FORMAT_CHWN) ? 3 : 0;
		if (hw >= 0)
			for (i = 0; i < CCV_NNC_MAX_DIM; i++)
				batch_norm.bnorm.axis[i + 1] = i + hw;
		self->bnorm.params = batch_norm;
		self->bnorm.exec = ccv_nnc_graph_exec_symbol_new(graph, ccv_nnc_cmd(CCV_NNC_BATCH_NORM_FORWARD, 0, batch_norm, 0), TENSOR_SYMBOL_LIST(inputs[0], self->scale, self->bias, mean, var), TENSOR_SYMBOL_LIST(output, out_mean, out_var, saved_mean, saved_inv_std), 0);
	}
	if (self->params.activation == CCV_CNNP_ACTIVATION_RELU)
	{
		const ccv_nnc_tensor_symbol_t relu_output = ccv_nnc_tensor_symbol_new(graph, params, 0);
		ccv_nnc_graph_exec_symbol_new(graph, CMD_RELU_FORWARD(), TENSOR_SYMBOL_LIST(output), TENSOR_SYMBOL_LIST(relu_output), 0);
		outputs[0] = relu_output;
	} else if (self->params.activation == CCV_CNNP_ACTIVATION_SOFTMAX) {
		const ccv_nnc_tensor_symbol_t softmax_output = ccv_nnc_tensor_symbol_new(graph, params, 0);
		ccv_nnc_graph_exec_symbol_new(graph, CMD_SOFTMAX_FORWARD(), TENSOR_SYMBOL_LIST(output), TENSOR_SYMBOL_LIST(softmax_output), 0);
		outputs[0] = softmax_output;
	} else
		outputs[0] = output;
}

static void _ccv_cnnp_identity_init_states(ccv_cnnp_model_t* const super, ccv_nnc_symbolic_graph_t* const graph, const ccv_cnnp_state_initializer_f initializer, void* const context)
{
	ccv_cnnp_model_identity_t* const self = (ccv_cnnp_model_identity_t*)super;
	if (self->bias.graph)
		initializer(context, CMD_SET_FORWARD(0), ccv_nnc_no_hint, 0, 0, self->bias);
	if (self->scale.graph)
		initializer(context, CMD_RANDOM_UNIFORM_FORWARD(0, 1), ccv_nnc_no_hint, 0, 0, self->scale);
	int i;
	if (self->zero_inits)
		for (i = 0; i < self->zero_inits->rnum; i++)
			initializer(context, CMD_SET_FORWARD(0), ccv_nnc_no_hint, 0, 0, *(ccv_nnc_tensor_symbol_t*)ccv_array_get(self->zero_inits, i));
}

static void _ccv_cnnp_identity_add_to_trainable(ccv_cnnp_model_t* const super, ccv_array_t* const trainables)
{
	ccv_cnnp_model_identity_t* const self = (ccv_cnnp_model_identity_t*)super;
	if (self->bias.graph)
		ccv_array_push(trainables, &self->bias);
	if (self->scale.graph)
		ccv_array_push(trainables, &self->scale);
}

static void _ccv_cnnp_identity_add_to_output(ccv_cnnp_model_t* const super, ccv_array_t* const outputs)
{
	ccv_cnnp_model_identity_t* const self = (ccv_cnnp_model_identity_t*)super;
	int i;
	if (self->retainables)
		for (i = 0; i < self->retainables->rnum; i++)
		{
			const ccv_nnc_tensor_symbol_t symbol = *(ccv_nnc_tensor_symbol_t*)ccv_array_get(self->retainables, i);
			ccv_array_push(outputs, &symbol);
		}
}

static void _ccv_cnnp_identity_set_is_test(ccv_cnnp_model_t* const super, const int is_test, const ccv_cnnp_cmd_updater_f updater, void* const context)
{
	ccv_cnnp_model_identity_t* const self = (ccv_cnnp_model_identity_t*)super;
	if (self->bnorm.exec.graph)
	{
		self->bnorm.params.bnorm.is_test = is_test;
		updater(context, self->bnorm.exec, ccv_nnc_cmd(CCV_NNC_BATCH_NORM_FORWARD, 0, self->bnorm.params, 0), ccv_nnc_no_hint);
	}
}

static void _ccv_cnnp_identity_deinit(ccv_cnnp_model_t* const super)
{
	ccv_cnnp_model_identity_t* const self = (ccv_cnnp_model_identity_t*)super;
	if (self->zero_inits)
		ccv_array_free(self->zero_inits);
	if (self->retainables)
		ccv_array_free(self->retainables);
}

static const ccv_cnnp_model_vtab_t ccv_cnnp_identity_isa = {
	.build = _ccv_cnnp_identity_build,
	.init_states = _ccv_cnnp_identity_init_states,
	.add_to_trainable = _ccv_cnnp_identity_add_to_trainable,
	.add_to_output = _ccv_cnnp_identity_add_to_output,
	.set_is_test = _ccv_cnnp_identity_set_is_test,
	.deinit = _ccv_cnnp_identity_deinit,
};

ccv_cnnp_model_t* ccv_cnnp_identity(const ccv_cnnp_param_t params)
{
	ccv_cnnp_model_identity_t* const model_identity = (ccv_cnnp_model_identity_t*)cccalloc(1, sizeof(ccv_cnnp_model_identity_t));
	model_identity->super.isa = &ccv_cnnp_identity_isa;
	model_identity->super.input_size = 1;
	model_identity->super.outputs = &model_identity->output;
	model_identity->super.output_size = 1;
	model_identity->bias.d = CCV_NNC_NO_TENSOR_SYMBOL;
	model_identity->bias.graph = 0;
	model_identity->params = params;
	return (ccv_cnnp_model_t*)model_identity;
}

#pragma mark - Convolution Layer

typedef struct {
	ccv_cnnp_model_t super;
	ccv_nnc_tensor_symbol_t output;
	ccv_nnc_tensor_symbol_t weights;
	ccv_nnc_tensor_symbol_t bias;
	ccv_nnc_tensor_symbol_t scale;
	struct {
		ccv_nnc_graph_exec_symbol_t exec;
		ccv_nnc_cmd_param_t params;
	} bnorm;
	ccv_array_t* zero_inits;
	ccv_array_t* retainables;
	int groups;
	int filters;
	int kdim[CCV_NNC_MAX_DIM_ALLOC];
	ccv_cnnp_param_t params;
} ccv_cnnp_model_convolution_t;

static void _ccv_cnnp_convolution_build(ccv_cnnp_model_t* const super, ccv_nnc_symbolic_graph_t* const graph, const ccv_nnc_tensor_symbol_t* const inputs, const int input_size, ccv_nnc_tensor_symbol_t* const outputs, const int output_size)
{
	assert(input_size == 1);
	assert(output_size == 1);
	ccv_cnnp_model_convolution_t* const self = (ccv_cnnp_model_convolution_t*)super;
	const ccv_nnc_tensor_param_t params = ccv_nnc_tensor_symbol_params(graph, inputs[0]);
	int i;
	const int nd = CCV_NNC_MAX_DIM + 2;
	ccv_nnc_tensor_param_t weights_params = params;
	ccv_nnc_tensor_set_n(&weights_params, self->filters);
	assert(ccv_nnc_tensor_get_c(params) % self->groups == 0);
	ccv_nnc_tensor_set_c(&weights_params, nd, ccv_nnc_tensor_get_c(params) / self->groups);
	const int hw = ccv_nnc_tensor_hw(weights_params, nd);
	assert(hw >= 0);
	for (i = 0; i < CCV_NNC_MAX_DIM; i++)
		weights_params.dim[i + hw] = self->kdim[i];
	if (!self->weights.graph)
		self->weights = ccv_nnc_tensor_symbol_new(graph, weights_params, 0);
	assert(self->weights.graph == graph);
	ccv_nnc_tensor_param_t bias_params = params;
	memset(bias_params.dim, 0, sizeof(bias_params.dim));
	bias_params.dim[0] = self->filters;
	ccv_nnc_cmd_t cmd = CMD_CONVOLUTION_FORWARD(self->groups, self->filters);
	for (i = 0; i < CCV_NNC_MAX_DIM; i++)
		cmd.info.size.dim[i] = self->kdim[i];
	ccv_nnc_tensor_param_t output_params;
	ccv_nnc_hint_tensor_auto(cmd, (ccv_nnc_tensor_param_t []){
			params,
			weights_params,
			bias_params,
		}, 3, self->params.hint, &output_params, 1);
	const ccv_nnc_tensor_symbol_t output = ccv_nnc_tensor_symbol_new(graph, output_params, 0);
	if (self->params.norm == CCV_CNNP_BATCH_NORM)
	{
		const ccv_nnc_tensor_symbol_t convolution_output = ccv_nnc_tensor_symbol_new(graph, output_params, 0);
		const ccv_nnc_graph_exec_symbol_t convolution = ccv_nnc_graph_exec_symbol_new(graph, cmd, TENSOR_SYMBOL_LIST(inputs[0], self->weights), TENSOR_SYMBOL_LIST(convolution_output), 0);
		ccv_nnc_graph_exec_symbol_set_hint(graph, convolution, self->params.hint);
		// If the accuracy is not enough, bump it to 32-bit floating point.
		if (bias_params.datatype != CCV_32F || bias_params.datatype != CCV_64F)
			bias_params.datatype = CCV_32F;
		// Both scale and bias are shared between if this model is reused.
		if (!self->scale.graph)
			self->scale = ccv_nnc_tensor_symbol_new(graph, bias_params, 0);
		if (!self->bias.graph)
			self->bias = ccv_nnc_tensor_symbol_new(graph, bias_params, 0);
		const ccv_nnc_tensor_symbol_t mean = ccv_nnc_tensor_symbol_new(graph, bias_params, 0);
		const ccv_nnc_tensor_symbol_t var = ccv_nnc_tensor_symbol_new(graph, bias_params, 0);
		// Otherwise, notice mean, var, saved_mean, saved_inv_std are not reused.
		if (!self->zero_inits)
			self->zero_inits = ccv_array_new(sizeof(ccv_nnc_tensor_symbol_t), 0, 0);
		ccv_array_push(self->zero_inits, &mean);
		ccv_array_push(self->zero_inits, &var);
		const ccv_nnc_tensor_symbol_t out_mean = ccv_nnc_tensor_symbol_new(graph, bias_params, 0);
		const ccv_nnc_tensor_symbol_t out_var = ccv_nnc_tensor_symbol_new(graph, bias_params, 0);
		if (!self->retainables)
			self->retainables = ccv_array_new(sizeof(ccv_nnc_tensor_symbol_t), 0, 0);
		ccv_array_push(self->retainables, &out_mean);
		ccv_array_push(self->retainables, &out_var);
		const ccv_nnc_tensor_symbol_t saved_mean = ccv_nnc_tensor_symbol_new(graph, bias_params, 0);
		const ccv_nnc_tensor_symbol_t saved_inv_std = ccv_nnc_tensor_symbol_new(graph, bias_params, 0);
		const int hw = ccv_nnc_tensor_hw(output_params, ccv_nnc_tensor_nd(output_params.dim));
		ccv_nnc_cmd_param_t batch_norm = {
			.bnorm = {
				.epsilon = 1e-4,
				.is_test = 0,
				.momentum = 0.9,
				.count = hw >= 0 ? CCV_NNC_MAX_DIM + 1 : 1,
			}
		};
		batch_norm.bnorm.axis[0] = (output_params.format == CCV_TENSOR_FORMAT_CHWN) ? 3 : 0;
		if (hw >= 0)
			for (i = 0; i < CCV_NNC_MAX_DIM; i++)
				batch_norm.bnorm.axis[i + 1] = i + hw;
		self->bnorm.params = batch_norm;
		self->bnorm.exec = ccv_nnc_graph_exec_symbol_new(graph, ccv_nnc_cmd(CCV_NNC_BATCH_NORM_FORWARD, 0, batch_norm, 0), TENSOR_SYMBOL_LIST(convolution_output, self->scale, self->bias, mean, var), TENSOR_SYMBOL_LIST(output, out_mean, out_var, saved_mean, saved_inv_std), 0);
	} else {
		ccv_nnc_graph_exec_symbol_t convolution;
		if (self->params.no_bias)
			convolution = ccv_nnc_graph_exec_symbol_new(graph, cmd, TENSOR_SYMBOL_LIST(inputs[0], self->weights), TENSOR_SYMBOL_LIST(output), 0);
		else {
			if (!self->bias.graph)
				self->bias = ccv_nnc_tensor_symbol_new(graph, bias_params, 0);
			convolution = ccv_nnc_graph_exec_symbol_new(graph, cmd, TENSOR_SYMBOL_LIST(inputs[0], self->weights, self->bias), TENSOR_SYMBOL_LIST(output), 0);
		}
		ccv_nnc_graph_exec_symbol_set_hint(graph, convolution, self->params.hint);
	}
	if (self->params.activation == CCV_CNNP_ACTIVATION_RELU)
	{
		const ccv_nnc_tensor_symbol_t relu_output = ccv_nnc_tensor_symbol_new(graph, output_params, 0);
		ccv_nnc_graph_exec_symbol_new(graph, CMD_RELU_FORWARD(), TENSOR_SYMBOL_LIST(output), TENSOR_SYMBOL_LIST(relu_output), 0);
		outputs[0] = relu_output;
	} else if (self->params.activation == CCV_CNNP_ACTIVATION_SOFTMAX) {
		const ccv_nnc_tensor_symbol_t softmax_output = ccv_nnc_tensor_symbol_new(graph, output_params, 0);
		ccv_nnc_graph_exec_symbol_new(graph, CMD_SOFTMAX_FORWARD(), TENSOR_SYMBOL_LIST(output), TENSOR_SYMBOL_LIST(softmax_output), 0);
		outputs[0] = softmax_output;
	} else
		outputs[0] = output;
}

static void _ccv_cnnp_convolution_init_states(ccv_cnnp_model_t* const super, ccv_nnc_symbolic_graph_t* const graph, const ccv_cnnp_state_initializer_f initializer, void* const context)
{
	ccv_cnnp_model_convolution_t* const self = (ccv_cnnp_model_convolution_t*)super;
	const ccv_nnc_tensor_param_t weight_params = ccv_nnc_tensor_symbol_params(graph, self->weights);
	const int n = ccv_max(ccv_nnc_tensor_get_n(weight_params), 1);
	const int count = ccv_nnc_tensor_count(weight_params);
	const float std = sqrtf(2) / sqrtf(count / n);
	const float bound = sqrtf(3) * std;
	initializer(context, CMD_RANDOM_UNIFORM_FORWARD(-bound, bound), ccv_nnc_no_hint, 0, 0, self->weights);
	if (self->bias.graph)
		initializer(context, CMD_SET_FORWARD(0), ccv_nnc_no_hint, 0, 0, self->bias);
	if (self->scale.graph)
		initializer(context, CMD_RANDOM_UNIFORM_FORWARD(0, 1), ccv_nnc_no_hint, 0, 0, self->scale);
	int i;
	if (self->zero_inits)
		for (i = 0; i < self->zero_inits->rnum; i++)
			initializer(context, CMD_SET_FORWARD(0), ccv_nnc_no_hint, 0, 0, *(ccv_nnc_tensor_symbol_t*)ccv_array_get(self->zero_inits, i));
}

static void _ccv_cnnp_convolution_add_to_trainable(ccv_cnnp_model_t* const super, ccv_array_t* const trainables)
{
	ccv_cnnp_model_convolution_t* const self = (ccv_cnnp_model_convolution_t*)super;
	ccv_array_push(trainables, &self->weights);
	if (self->bias.graph)
		ccv_array_push(trainables, &self->bias);
	if (self->scale.graph)
		ccv_array_push(trainables, &self->scale);
}

static void _ccv_cnnp_convolution_add_to_output(ccv_cnnp_model_t* const super, ccv_array_t* const outputs)
{
	ccv_cnnp_model_convolution_t* const self = (ccv_cnnp_model_convolution_t*)super;
	int i;
	if (self->retainables)
		for (i = 0; i < self->retainables->rnum; i++)
		{
			const ccv_nnc_tensor_symbol_t symbol = *(ccv_nnc_tensor_symbol_t*)ccv_array_get(self->retainables, i);
			ccv_array_push(outputs, &symbol);
		}
}

static void _ccv_cnnp_convolution_set_is_test(ccv_cnnp_model_t* const super, const int is_test, const ccv_cnnp_cmd_updater_f updater, void* const context)
{
	ccv_cnnp_model_convolution_t* const self = (ccv_cnnp_model_convolution_t*)super;
	if (self->bnorm.exec.graph)
	{
		self->bnorm.params.bnorm.is_test = is_test;
		updater(context, self->bnorm.exec, ccv_nnc_cmd(CCV_NNC_BATCH_NORM_FORWARD, 0, self->bnorm.params, 0), ccv_nnc_no_hint);
	}
}

static void _ccv_cnnp_convolution_deinit(ccv_cnnp_model_t* const super)
{
	ccv_cnnp_model_convolution_t* const self = (ccv_cnnp_model_convolution_t*)super;
	if (self->zero_inits)
		ccv_array_free(self->zero_inits);
	if (self->retainables)
		ccv_array_free(self->retainables);
}

static const ccv_cnnp_model_vtab_t ccv_cnnp_convolution_isa = {
	.build = _ccv_cnnp_convolution_build,
	.init_states = _ccv_cnnp_convolution_init_states,
	.add_to_trainable = _ccv_cnnp_convolution_add_to_trainable,
	.add_to_output = _ccv_cnnp_convolution_add_to_output,
	.set_is_test = _ccv_cnnp_convolution_set_is_test,
	.deinit = _ccv_cnnp_convolution_deinit,
};

ccv_cnnp_model_t* ccv_cnnp_convolution(const int groups, const int filters, const int kdim[CCV_NNC_MAX_DIM_ALLOC], const ccv_cnnp_param_t params)
{
	ccv_cnnp_model_convolution_t* const model_convolution = (ccv_cnnp_model_convolution_t*)cccalloc(1, sizeof(ccv_cnnp_model_convolution_t));
	model_convolution->super.isa = &ccv_cnnp_convolution_isa;
	model_convolution->super.input_size = 1;
	model_convolution->super.outputs = &model_convolution->output;
	model_convolution->super.output_size = 1;
	model_convolution->weights.d = CCV_NNC_NO_TENSOR_SYMBOL;
	model_convolution->weights.graph = 0;
	model_convolution->bias.d = CCV_NNC_NO_TENSOR_SYMBOL;
	model_convolution->bias.graph = 0;
	model_convolution->groups = groups;
	model_convolution->filters = filters;
	memcpy(model_convolution->kdim, kdim, sizeof(model_convolution->kdim));
	model_convolution->params = params;
	return (ccv_cnnp_model_t*)model_convolution;
}

#pragma mark - Dense Layer

typedef struct {
	ccv_cnnp_model_t super;
	ccv_nnc_tensor_symbol_t output;
	ccv_nnc_tensor_symbol_t weights;
	ccv_nnc_tensor_symbol_t bias;
	ccv_nnc_tensor_symbol_t scale;
	struct {
		ccv_nnc_graph_exec_symbol_t exec;
		ccv_nnc_cmd_param_t params;
	} bnorm;
	ccv_array_t* zero_inits;
	ccv_array_t* retainables;
	int count;
	ccv_cnnp_param_t params;
} ccv_cnnp_model_dense_t;

static void _ccv_cnnp_dense_build(ccv_cnnp_model_t* const super, ccv_nnc_symbolic_graph_t* const graph, const ccv_nnc_tensor_symbol_t* const inputs, const int input_size, ccv_nnc_tensor_symbol_t* const outputs, const int output_size)
{
	assert(input_size == 1);
	assert(output_size == 1);
	ccv_cnnp_model_dense_t* const self = (ccv_cnnp_model_dense_t*)super;
	const ccv_nnc_tensor_param_t params = ccv_nnc_tensor_symbol_params(graph, inputs[0]);
	ccv_nnc_tensor_param_t weights_params = params;
	memset(weights_params.dim, 0, sizeof(weights_params.dim));
	weights_params.dim[0] = self->count;
	weights_params.dim[1] = ccv_nnc_tensor_get_c(params);
	if (!self->weights.graph)
		self->weights = ccv_nnc_tensor_symbol_new(graph, weights_params, 0);
	assert(self->weights.graph == graph);
	ccv_nnc_tensor_param_t bias_params = params;
	memset(bias_params.dim, 0, sizeof(bias_params.dim));
	bias_params.dim[0] = self->count;
	const ccv_nnc_cmd_t cmd = CMD_GEMM_FORWARD(self->count);
	ccv_nnc_tensor_param_t output_params;
	ccv_nnc_hint_tensor_auto(cmd, (ccv_nnc_tensor_param_t []){
			params,
			weights_params,
			bias_params,
		}, 3, self->params.hint, &output_params, 1);
	const ccv_nnc_tensor_symbol_t output = ccv_nnc_tensor_symbol_new(graph, output_params, 0);
	if (self->params.norm == CCV_CNNP_BATCH_NORM)
	{
		const ccv_nnc_tensor_symbol_t dense_output = ccv_nnc_tensor_symbol_new(graph, output_params, 0);
		ccv_nnc_graph_exec_symbol_new(graph, cmd, TENSOR_SYMBOL_LIST(inputs[0], self->weights), TENSOR_SYMBOL_LIST(dense_output), 0);
		// If the accuracy is not enough, bump it to 32-bit floating point.
		if (bias_params.datatype != CCV_32F || bias_params.datatype != CCV_64F)
			bias_params.datatype = CCV_32F;
		// Both scale and bias are shared between if this model is reused.
		if (!self->scale.graph)
			self->scale = ccv_nnc_tensor_symbol_new(graph, bias_params, 0);
		if (!self->bias.graph)
			self->bias = ccv_nnc_tensor_symbol_new(graph, bias_params, 0);
		const ccv_nnc_tensor_symbol_t mean = ccv_nnc_tensor_symbol_new(graph, bias_params, 0);
		const ccv_nnc_tensor_symbol_t var = ccv_nnc_tensor_symbol_new(graph, bias_params, 0);
		// Otherwise, notice mean, var, saved_mean, saved_inv_std are not reused.
		if (!self->zero_inits)
			self->zero_inits = ccv_array_new(sizeof(ccv_nnc_tensor_symbol_t), 0, 0);
		ccv_array_push(self->zero_inits, &mean);
		ccv_array_push(self->zero_inits, &var);
		const ccv_nnc_tensor_symbol_t out_mean = ccv_nnc_tensor_symbol_new(graph, bias_params, 0);
		const ccv_nnc_tensor_symbol_t out_var = ccv_nnc_tensor_symbol_new(graph, bias_params, 0);
		if (!self->retainables)
			self->retainables = ccv_array_new(sizeof(ccv_nnc_tensor_symbol_t), 0, 0);
		ccv_array_push(self->retainables, &out_mean);
		ccv_array_push(self->retainables, &out_var);
		const ccv_nnc_tensor_symbol_t saved_mean = ccv_nnc_tensor_symbol_new(graph, bias_params, 0);
		const ccv_nnc_tensor_symbol_t saved_inv_std = ccv_nnc_tensor_symbol_new(graph, bias_params, 0);
		const int hw = ccv_nnc_tensor_hw(output_params, ccv_nnc_tensor_nd(output_params.dim));
		ccv_nnc_cmd_param_t batch_norm = {
			.bnorm = {
				.epsilon = 1e-4,
				.is_test = 0,
				.momentum = 0.9,
				.count = hw >= 0 ? CCV_NNC_MAX_DIM + 1 : 1,
			}
		};
		int i;
		batch_norm.bnorm.axis[0] = (output_params.format == CCV_TENSOR_FORMAT_CHWN) ? 3 : 0;
		if (hw >= 0)
			for (i = 0; i < CCV_NNC_MAX_DIM; i++)
				batch_norm.bnorm.axis[i + 1] = i + hw;
		self->bnorm.params = batch_norm;
		self->bnorm.exec = ccv_nnc_graph_exec_symbol_new(graph, ccv_nnc_cmd(CCV_NNC_BATCH_NORM_FORWARD, 0, batch_norm, 0), TENSOR_SYMBOL_LIST(dense_output, self->scale, self->bias, mean, var), TENSOR_SYMBOL_LIST(output, out_mean, out_var, saved_mean, saved_inv_std), 0);
	} else {
		if (self->params.no_bias)
			ccv_nnc_graph_exec_symbol_new(graph, cmd, TENSOR_SYMBOL_LIST(inputs[0], self->weights), TENSOR_SYMBOL_LIST(output), 0);
		else {
			if (!self->bias.graph)
				self->bias = ccv_nnc_tensor_symbol_new(graph, bias_params, 0);
			ccv_nnc_graph_exec_symbol_new(graph, cmd, TENSOR_SYMBOL_LIST(inputs[0], self->weights, self->bias), TENSOR_SYMBOL_LIST(output), 0);
		}
	}
	if (self->params.activation == CCV_CNNP_ACTIVATION_RELU)
	{
		const ccv_nnc_tensor_symbol_t relu_output = ccv_nnc_tensor_symbol_new(graph, output_params, 0);
		ccv_nnc_graph_exec_symbol_new(graph, CMD_RELU_FORWARD(), TENSOR_SYMBOL_LIST(output), TENSOR_SYMBOL_LIST(relu_output), 0);
		outputs[0] = relu_output;
	} else if (self->params.activation == CCV_CNNP_ACTIVATION_SOFTMAX) {
		const ccv_nnc_tensor_symbol_t softmax_output = ccv_nnc_tensor_symbol_new(graph, output_params, 0);
		ccv_nnc_graph_exec_symbol_new(graph, CMD_SOFTMAX_FORWARD(), TENSOR_SYMBOL_LIST(output), TENSOR_SYMBOL_LIST(softmax_output), 0);
		outputs[0] = softmax_output;
	} else
		outputs[0] = output;
}

static void _ccv_cnnp_dense_init_states(ccv_cnnp_model_t* const super, ccv_nnc_symbolic_graph_t* const graph, const ccv_cnnp_state_initializer_f initializer, void* const context)
{
	ccv_cnnp_model_dense_t* const self = (ccv_cnnp_model_dense_t*)super;
	const ccv_nnc_tensor_param_t weight_params = ccv_nnc_tensor_symbol_params(graph, self->weights);
	const int c = weight_params.dim[1];
	const float std = sqrtf(2) / sqrtf(c);
	const float bound = sqrtf(3) * std;
	initializer(context, CMD_RANDOM_UNIFORM_FORWARD(-bound, bound), ccv_nnc_no_hint, 0, 0, self->weights);
	if (self->bias.graph)
		initializer(context, CMD_SET_FORWARD(0), ccv_nnc_no_hint, 0, 0, self->bias);
	if (self->scale.graph)
		initializer(context, CMD_RANDOM_UNIFORM_FORWARD(0, 1), ccv_nnc_no_hint, 0, 0, self->scale);
	int i;
	if (self->zero_inits)
		for (i = 0; i < self->zero_inits->rnum; i++)
			initializer(context, CMD_SET_FORWARD(0), ccv_nnc_no_hint, 0, 0, *(ccv_nnc_tensor_symbol_t*)ccv_array_get(self->zero_inits, i));
}

static void _ccv_cnnp_dense_add_to_trainable(ccv_cnnp_model_t* const super, ccv_array_t* const trainables)
{
	ccv_cnnp_model_dense_t* const self = (ccv_cnnp_model_dense_t*)super;
	ccv_array_push(trainables, &self->weights);
	if (self->bias.graph)
		ccv_array_push(trainables, &self->bias);
	if (self->scale.graph)
		ccv_array_push(trainables, &self->scale);
}

static void _ccv_cnnp_dense_add_to_output(ccv_cnnp_model_t* const super, ccv_array_t* const outputs)
{
	ccv_cnnp_model_dense_t* const self = (ccv_cnnp_model_dense_t*)super;
	int i;
	if (self->retainables)
		for (i = 0; i < self->retainables->rnum; i++)
		{
			const ccv_nnc_tensor_symbol_t symbol = *(ccv_nnc_tensor_symbol_t*)ccv_array_get(self->retainables, i);
			ccv_array_push(outputs, &symbol);
		}
}

static void _ccv_cnnp_dense_set_is_test(ccv_cnnp_model_t* const super, const int is_test, const ccv_cnnp_cmd_updater_f updater, void* const context)
{
	ccv_cnnp_model_dense_t* const self = (ccv_cnnp_model_dense_t*)super;
	if (self->bnorm.exec.graph)
	{
		self->bnorm.params.bnorm.is_test = is_test;
		updater(context, self->bnorm.exec, ccv_nnc_cmd(CCV_NNC_BATCH_NORM_FORWARD, 0, self->bnorm.params, 0), ccv_nnc_no_hint);
	}
}

static void _ccv_cnnp_dense_deinit(ccv_cnnp_model_t* const super)
{
	ccv_cnnp_model_dense_t* const self = (ccv_cnnp_model_dense_t*)super;
	if (self->zero_inits)
		ccv_array_free(self->zero_inits);
	if (self->retainables)
		ccv_array_free(self->retainables);
}

static const ccv_cnnp_model_vtab_t ccv_cnnp_dense_isa = {
	.build = _ccv_cnnp_dense_build,
	.init_states = _ccv_cnnp_dense_init_states,
	.add_to_trainable = _ccv_cnnp_dense_add_to_trainable,
	.add_to_output = _ccv_cnnp_dense_add_to_output,
	.set_is_test = _ccv_cnnp_dense_set_is_test,
	.deinit = _ccv_cnnp_dense_deinit,
};

ccv_cnnp_model_t* ccv_cnnp_dense(const int count, const ccv_cnnp_param_t params)
{
	ccv_cnnp_model_dense_t* const model_dense = (ccv_cnnp_model_dense_t*)cccalloc(1, sizeof(ccv_cnnp_model_dense_t));
	model_dense->super.isa = &ccv_cnnp_dense_isa;
	model_dense->super.input_size = 1;
	model_dense->super.outputs = &model_dense->output;
	model_dense->super.output_size = 1;
	model_dense->weights.d = CCV_NNC_NO_TENSOR_SYMBOL;
	model_dense->weights.graph = 0;
	model_dense->bias.d = CCV_NNC_NO_TENSOR_SYMBOL;
	model_dense->bias.graph = 0;
	model_dense->count = count;
	model_dense->params = params;
	return (ccv_cnnp_model_t*)model_dense;
}

#pragma mark - Pool Layers

typedef struct {
	ccv_cnnp_model_t super;
	ccv_nnc_tensor_symbol_t output;
	int kdim[CCV_NNC_MAX_DIM_ALLOC];
	ccv_cnnp_param_t params;
} ccv_cnnp_model_pool_t;

static void _ccv_cnnp_max_pool_build(ccv_cnnp_model_t* const super, ccv_nnc_symbolic_graph_t* const graph, const ccv_nnc_tensor_symbol_t* const inputs, const int input_size, ccv_nnc_tensor_symbol_t* const outputs, const int output_size)
{
	assert(input_size == 1);
	assert(output_size == 1);
	ccv_cnnp_model_pool_t* const self = (ccv_cnnp_model_pool_t*)super;
	const ccv_nnc_tensor_param_t params = ccv_nnc_tensor_symbol_params(graph, inputs[0]);
	const int hw = ccv_nnc_tensor_hw(params, ccv_nnc_tensor_nd(params.dim));
	ccv_nnc_cmd_t cmd;
	if (hw >= 0 && self->kdim[0] == 0 && self->kdim[1] == 0)
		cmd = CMD_MAX_POOL_FORWARD(params.dim[hw], params.dim[hw + 1]);
	else
		cmd = CMD_MAX_POOL_FORWARD(self->kdim[0], self->kdim[1]);
	ccv_nnc_tensor_param_t output_params;
	ccv_nnc_hint_tensor_auto(cmd, &params, 1, self->params.hint, &output_params, 1);
	const ccv_nnc_tensor_symbol_t pool_output = ccv_nnc_tensor_symbol_new(graph, output_params, 0);
	const ccv_nnc_graph_exec_symbol_t exec = ccv_nnc_graph_exec_symbol_new(graph, cmd, TENSOR_SYMBOL_LIST(inputs[0]), TENSOR_SYMBOL_LIST(pool_output), 0);
	ccv_nnc_graph_exec_symbol_set_hint(graph, exec, self->params.hint);
	outputs[0] = pool_output;
}

static const ccv_cnnp_model_vtab_t ccv_cnnp_max_pool_isa = {
	.build = _ccv_cnnp_max_pool_build,
};

ccv_cnnp_model_t* ccv_cnnp_max_pool(const int kdim[CCV_NNC_MAX_DIM_ALLOC], const ccv_cnnp_param_t params)
{
	ccv_cnnp_model_pool_t* const model_pool = (ccv_cnnp_model_pool_t*)cccalloc(1, sizeof(ccv_cnnp_model_pool_t));
	model_pool->super.isa = &ccv_cnnp_max_pool_isa;
	model_pool->super.input_size = 1;
	model_pool->super.outputs = &model_pool->output;
	model_pool->super.output_size = 1;
	memcpy(model_pool->kdim, kdim, sizeof(model_pool->kdim));
	model_pool->params = params;
	return (ccv_cnnp_model_t*)model_pool;
}

static void _ccv_cnnp_average_pool_build(ccv_cnnp_model_t* const super, ccv_nnc_symbolic_graph_t* const graph, const ccv_nnc_tensor_symbol_t* const inputs, const int input_size, ccv_nnc_tensor_symbol_t* const outputs, const int output_size)
{
	assert(input_size == 1);
	assert(output_size == 1);
	ccv_cnnp_model_pool_t* const self = (ccv_cnnp_model_pool_t*)super;
	const ccv_nnc_tensor_param_t params = ccv_nnc_tensor_symbol_params(graph, inputs[0]);
	const int hw = ccv_nnc_tensor_hw(params, ccv_nnc_tensor_nd(params.dim));
	ccv_nnc_cmd_t cmd;
	if (hw >= 0 && self->kdim[0] == 0 && self->kdim[1] == 0)
		cmd = CMD_AVERAGE_POOL_FORWARD(params.dim[hw], params.dim[hw + 1]);
	else
		cmd = CMD_AVERAGE_POOL_FORWARD(self->kdim[0], self->kdim[1]);
	ccv_nnc_tensor_param_t output_params;
	ccv_nnc_hint_tensor_auto(cmd, &params, 1, self->params.hint, &output_params, 1);
	const ccv_nnc_tensor_symbol_t pool_output = ccv_nnc_tensor_symbol_new(graph, output_params, 0);
	const ccv_nnc_graph_exec_symbol_t exec = ccv_nnc_graph_exec_symbol_new(graph, cmd, TENSOR_SYMBOL_LIST(inputs[0]), TENSOR_SYMBOL_LIST(pool_output), 0);
	ccv_nnc_graph_exec_symbol_set_hint(graph, exec, self->params.hint);
	outputs[0] = pool_output;
}

static const ccv_cnnp_model_vtab_t ccv_cnnp_average_pool_isa = {
	.build = _ccv_cnnp_average_pool_build,
};

ccv_cnnp_model_t* ccv_cnnp_average_pool(const int kdim[CCV_NNC_MAX_DIM_ALLOC], const ccv_cnnp_param_t params)
{
	ccv_cnnp_model_pool_t* const model_pool = (ccv_cnnp_model_pool_t*)cccalloc(1, sizeof(ccv_cnnp_model_pool_t));
	model_pool->super.isa = &ccv_cnnp_average_pool_isa;
	model_pool->super.input_size = 1;
	model_pool->super.outputs = &model_pool->output;
	model_pool->super.output_size = 1;
	memcpy(model_pool->kdim, kdim, sizeof(model_pool->kdim));
	model_pool->params = params;
	return (ccv_cnnp_model_t*)model_pool;
}

#pragma mark - Command Layer

typedef struct {
	ccv_cnnp_model_t super;
	ccv_nnc_cmd_t cmd;
	ccv_nnc_hint_t hint;
	ccv_nnc_tensor_symbol_t* input_symbols; // This is only valid for INIT_SHARED_TENSOR / INIT_SHARED_TENSOR_AS_TRAINABLE
	ccv_nnc_tensor_symbol_t* output_symbols; // This is just for the output symbol (in case we need to have no tensor symbol).
	ccv_cnnp_tensor_param_t* inputs;
	int input_size;
	int* outputs;
	int output_size;
} ccv_cnnp_model_cmd_exec_t;

static void _ccv_cnnp_cmd_exec_build(ccv_cnnp_model_t* const super, ccv_nnc_symbolic_graph_t* const graph, const ccv_nnc_tensor_symbol_t* const inputs, const int input_size, ccv_nnc_tensor_symbol_t* const outputs, const int output_size)
{
	ccv_cnnp_model_cmd_exec_t* const self = (ccv_cnnp_model_cmd_exec_t*)super;
	ccv_nnc_tensor_param_t input_params[ccv_max(1, self->input_size)];
	int i, j;
	for (i = 0, j = 0; i < self->input_size; i++)
		if (self->inputs[i].type == CCV_CNNP_IO)
		{
			self->input_symbols[i] = inputs[j++];
			input_params[i] = ccv_nnc_tensor_symbol_params(graph, self->input_symbols[i]);
		} else if (self->inputs[i].type == CCV_CNNP_NO_TENSOR) {
			self->input_symbols[i] = NO_TENSOR_SYMBOL;
		} else if (!self->input_symbols[i].graph) {
			// Otherwise, we only create this symbol if it doesn't exist.
			const ccv_nnc_tensor_param_t params = self->inputs[i].tensor->info;
			input_params[i] = params;
			self->input_symbols[i] = ccv_nnc_tensor_symbol_new(graph, params, 0);
		}
	// We cannot simply mark the outputs as auto, because the subsequent build call may require this output to have params setup.
	// Infer the parameters here.
	ccv_nnc_tensor_param_t output_params[ccv_max(1, self->output_size)];
	ccv_nnc_hint_tensor_auto(self->cmd, input_params, self->input_size, self->hint, output_params, self->output_size);
	for (i = 0, j = 0; i < self->output_size; i++)
		if (self->outputs[i] == CCV_CNNP_IO)
			self->output_symbols[i] = outputs[j++] = ccv_nnc_tensor_symbol_new(graph, output_params[i], 0);
		else
			self->output_symbols[i] = NO_TENSOR_SYMBOL;
	ccv_nnc_graph_exec_symbol_new(graph, self->cmd, self->input_symbols, self->input_size, self->output_symbols, self->output_size, 0);
}

static void _ccv_cnnp_cmd_exec_init_states(ccv_cnnp_model_t* const super, ccv_nnc_symbolic_graph_t* const graph, const ccv_cnnp_state_initializer_f initializer, void* const context)
{
	ccv_cnnp_model_cmd_exec_t* const self = (ccv_cnnp_model_cmd_exec_t*)super;
	int i;
	for (i = 0; i < self->input_size; i++)
		if (self->inputs[i].type == CCV_CNNP_INIT_SHARED_TENSOR || self->inputs[i].type == CCV_CNNP_INIT_SHARED_TENSOR_AS_TRAINABLE)
			initializer(context, CMD_DATA_TRANSFER_FORWARD(), ccv_nnc_no_hint, 0, self->inputs[i].tensor, self->input_symbols[i]);
}

static void _ccv_cnnp_cmd_exec_add_to_trainable(ccv_cnnp_model_t* const super, ccv_array_t* const trainables)
{
	ccv_cnnp_model_cmd_exec_t* const self = (ccv_cnnp_model_cmd_exec_t*)super;
	int i;
	for (i = 0; i < self->input_size; i++)
		if (self->inputs[i].type == CCV_CNNP_INIT_SHARED_TENSOR_AS_TRAINABLE)
			ccv_array_push(trainables, &self->input_symbols[i]); // Push this as trainable.
}

static const ccv_cnnp_model_vtab_t ccv_cnnp_cmd_exec_isa = {
	.build = _ccv_cnnp_cmd_exec_build,
	.init_states = _ccv_cnnp_cmd_exec_init_states,
	.add_to_trainable = _ccv_cnnp_cmd_exec_add_to_trainable,
};

ccv_cnnp_model_t* ccv_cnnp_cmd_exec(const ccv_nnc_cmd_t cmd, const ccv_nnc_hint_t hint, const int flags, const ccv_cnnp_tensor_param_t* const inputs, const int input_size, const int* const outputs, const int output_size)
{
	assert(input_size >= 0);
	assert(output_size > 0);
	int i;
	int io_input_size = 0;
	for (i = 0; i < input_size; i++)
		if (inputs[i].type == CCV_CNNP_IO)
			++io_input_size;
		else {
			assert(inputs[i].type == CCV_CNNP_INIT_SHARED_TENSOR || inputs[i].type == CCV_CNNP_INIT_SHARED_TENSOR_AS_TRAINABLE);
			assert(inputs[i].tensor);
		}
	int io_output_size = 0;
	for (i = 0; i < output_size; i++)
		if (outputs[i] == CCV_CNNP_IO)
			++io_output_size;
		else {
			assert(outputs[i] == CCV_CNNP_NO_TENSOR);
		}
	assert(io_output_size > 0);
	ccv_cnnp_model_cmd_exec_t* const model_cmd_exec = (ccv_cnnp_model_cmd_exec_t*)cccalloc(1, sizeof(ccv_cnnp_model_cmd_exec_t) + sizeof(ccv_nnc_tensor_symbol_t) * (io_output_size + input_size + output_size) + sizeof(ccv_cnnp_tensor_param_t) * input_size + sizeof(int) * output_size);
	model_cmd_exec->super.isa = &ccv_cnnp_cmd_exec_isa;
	model_cmd_exec->super.input_size = io_input_size;
	model_cmd_exec->super.outputs = (ccv_nnc_tensor_symbol_t*)(model_cmd_exec + 1);
	model_cmd_exec->super.output_size = io_output_size;
	model_cmd_exec->cmd = cmd;
	model_cmd_exec->hint = hint;
	model_cmd_exec->input_size = input_size;
	model_cmd_exec->input_symbols = model_cmd_exec->super.outputs + io_output_size;
	model_cmd_exec->output_symbols = model_cmd_exec->input_symbols + input_size;
	model_cmd_exec->inputs = (ccv_cnnp_tensor_param_t*)(model_cmd_exec->output_symbols + output_size);
	if (input_size > 0)
		memcpy(model_cmd_exec->inputs, inputs, sizeof(ccv_cnnp_tensor_param_t) * input_size);
	model_cmd_exec->output_size = output_size;
	model_cmd_exec->outputs = (int*)(model_cmd_exec->inputs + input_size);
	if (output_size > 0)
		memcpy(model_cmd_exec->outputs, outputs, sizeof(int) * output_size);
	return (ccv_cnnp_model_t*)model_cmd_exec;
}
