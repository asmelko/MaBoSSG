library(ggplot2)
library(cowplot)
library(sitools)
library(viridis)
library(dplyr)


W=4.804
H=2
S=1
point_size=0.8
line_size=0.5
linecolors=scale_color_brewer(palette="Set1")
theme = theme_cowplot(font_size=7)
saturation_multiplier=4000

sisec=Vectorize(function(t)if(is.na(t))NA else sitools::f2si(t / 10^6, 's'))

average <- function(data)
{
    data["total"] = data["compilation"] + data["visualization"] + data["simulation"]
    data <- subset(data, select = -X)

    data_s <- split(data, paste0(data$max_time, data$nodes, data$formula_size, data$sample_count))
    data <- NULL
    for (i in 1:length(data_s)){
        tmp = subset(data_s[[i]], !(total %in% boxplot(data_s[[i]]$total, plot = FALSE)$out))
        data <- rbind(data, tmp)
    }

    data = data.frame(data %>% group_by_at(names(data)[-grep("(total)|(compilation)|(visualization)|(simulation)", names(data))]) %>% summarise(total = mean(total), compilation = mean(compilation), visualization = mean(visualization), simulation = mean(simulation)))
}

data_gpu = read.csv('gpu_out.csv', header=T, sep=';')
data_gpu = average(data_gpu)

data_v100 = read.csv('gpu_out_v100.csv', header=T, sep=';')
data_v100 = average(data_v100)

data_a100 = read.csv('gpu_out_a100.csv', header=T, sep=';')
data_a100 = average(data_a100)

data_cpu = read.csv('cpu_out.csv', header=T, sep=';')
names(data_cpu)[names(data_cpu) == "parsing"] <- "compilation"
data_cpu = subset(data_cpu, select = -threads)
data_cpu = average(data_cpu)

data_cpu["type"] = "Intel Xeon Gold 6130 CPU"
data_gpu["type"] = "NVIDIA RTX 3070 Laptop GPU"
data_v100["type"] = "NVIDIA Tesla V100 GPU"
data_a100["type"] = "NVIDIA Tesla A100 GPU"
data = rbind(data_cpu, data_gpu, data_v100, data_a100)

{
    data_c = data

    data_c = subset(data_c, max_time == 100)
    # data_c = subset(data_c, nodes <= 100)
    data_c = subset(data_c, formula_size == 4)
    data_c = subset(data_c, sample_count == 1000000)

    data_x = data_c
    data_y = data_c
    data_x["total"] = data_x["simulation"]
    data_x["desc"] = "Simulation"
    data_y["total"] = data_y["compilation"] + data_y["simulation"]
    data_y["desc"] = "Simulation + Runtime Compilation"

    data_c = rbind(data_x, data_y)

    ggsave("nodes.pdf", device='pdf', units="in", scale=S, width=W, height=H,
        ggplot(data_c, aes(x=nodes,y=total, color=factor(type), shape=factor(type))) +
        geom_point(size=point_size) +
        geom_line(linewidth=line_size) +
        xlab("Nodes count (log-scale)")+
        ylab("Wall-time (log-scale)")+
        labs(color="Machine", shape="Machine") +
        #scale_color_manual(values=RColorBrewer::brewer.pal(9,'YlGnBu')[2:9]) +
        scale_color_brewer(palette="Paired") +
        scale_y_log10(labels = sisec) +
        scale_x_log10(breaks=c(0, 10, 20, 40, 100, 200, 400, 1000)) +
        facet_wrap(~desc) +
        theme + background_grid() + theme(legend.position="bottom")
    )
}

{
    data_c = data

    data_c = subset(data_c, max_time == 100)
    # data_c = subset(data_c, nodes <= 100)
    data_c = subset(data_c, formula_size == 4)
    data_c = subset(data_c, sample_count == 1000000)

    data_c["total"] = data_c["compilation"] + data_c["simulation"]
    data_c["ratio"] = data_c["compilation"] / data_c["total"]

    ggsave("nodes-compilation-small.pdf", device='pdf', units="in", scale=S, width=W, height=H,
        ggplot(data_c, aes(x=nodes,y=ratio, color=factor(type), shape=factor(type))) +
        geom_point(size=point_size) +
        geom_line(linewidth=line_size) +
        xlab("Nodes count (log-scale)")+
        ylab("Wall-time (log-scale)")+
        labs(color="Machine", shape="Machine") +
        #scale_color_manual(values=RColorBrewer::brewer.pal(9,'YlGnBu')[2:9]) +
        scale_color_brewer(palette="Paired") +
        scale_y_continuous(labels = scales::percent) +
        scale_x_log10(breaks=c(0, 10, 20, 40, 100, 200, 400, 1000)) +
        # facet_wrap(~desc) +
        theme + background_grid() + theme(legend.position="bottom")
    )
}

{
    data_c = data

    data_c = subset(data_c, max_time == 100)
    data_c = subset(data_c, type == "NVIDIA RTX 3070 Laptop GPU")
    data_c = subset(data_c, nodes %in% c(200, 400, 600, 800, 1000))
    data_c = subset(data_c, formula_size %in% c(4, 9, 24, 49))
    data_c = subset(data_c, sample_count %in% c(4*10^6, 6*10^6, 8*10^6, 10*10^6))

    data_c["total"] = data_c["compilation"] + data_c["simulation"]
    data_c["ratio"] = data_c["compilation"] / data_c["total"]

    data_c["formula_size"] =  2 * (data_c["formula_size"] + 1)

    data_c$sample_count = factor(c("4M", "6M", "8M", "10M"), levels=c("4M", "6M", "8M", "10M"))

    ggsave("nodes-compilation-big.pdf", device='pdf', units="in", scale=S, width=W, height=H,
        ggplot(data_c, aes(x=nodes,y=ratio, color=sample_count, shape=sample_count)) +
        geom_point(size=point_size) +
        geom_line(linewidth=line_size) +
        xlab("Nodes count (log-scale)")+
        ylab("% of total time compiling (log-scale)")+
        labs(color="Sample Count", shape="Sample Count") +
        #scale_color_manual(values=RColorBrewer::brewer.pal(9,'YlGnBu')[2:9]) +
        scale_color_brewer(palette="Paired") +
        scale_y_log10(labels = scales::percent) +
        scale_x_continuous(breaks=c(200, 400, 600, 800, 1000)) +
        facet_wrap(~formula_size, ncol=4, labeller=labeller(formula_size=Vectorize(function(x) paste0("Formula size: ", x)))) +
        theme + background_grid() + theme(legend.position="bottom")
    )
}

{
    data_c = data

    data_c = subset(data_c, max_time == 100)
    data_c = subset(data_c, type == "NVIDIA RTX 3070 Laptop GPU")
    data_c = subset(data_c, nodes %in% c(200, 400, 600, 800, 1000))
    data_c = subset(data_c, formula_size %in% c(4, 9, 24, 49))
    data_c = subset(data_c, sample_count %in% c(4*10^6, 6*10^6, 8*10^6, 10*10^6))

    data_c["formula_size"] =  2 * (data_c["formula_size"] + 1)

    data_c$sample_count2 = factor(c("4M", "6M", "8M", "10M"), levels=c("4M", "6M", "8M", "10M"))

    ggsave("nodes-normalized.pdf", device='pdf', units="in", scale=S, width=W, height=H,
        ggplot(data_c, aes(x=nodes,y=simulation/sample_count/nodes/formula_size, color=sample_count2, shape=sample_count2)) +
        geom_point(size=point_size) +
        geom_line(linewidth=line_size) +
        xlab("Nodes count (log-scale)")+
        ylab("Normalized time per op (log-scale)")+
        labs(color="Sample Count", shape="Sample Count") +
        #scale_color_manual(values=RColorBrewer::brewer.pal(9,'YlGnBu')[2:9]) +
        scale_color_brewer(palette="Paired") +
        scale_y_log10(labels = sisec) +
        scale_x_continuous(breaks=c(200, 400, 600, 800, 1000)) +
        facet_wrap(~formula_size, ncol=4, labeller=labeller(formula_size=Vectorize(function(x) paste0("Formula size: ", x)))) +
        theme + background_grid() + theme(legend.position="bottom")
    )
}

{
    data_c = data

    data_c = subset(data_c, max_time == 100)
    data_c = subset(data_c, nodes == 100)
    data_c = subset(data_c, formula_size == 4)
    # data_c = subset(data_c, sample_count == 1000000)

    data_c["formula_size"] =  2 * (data_c["formula_size"] + 1)

    # data_c$sample_count2 = factor(c("1M", "2M", "3M", "4M", "5M", "6M", "7M", "8M", "9M", "10M"), levels=c("1M", "2M", "3M", "4M", "5M", "6M", "7M", "8M", "9M", "10M"))

    ggsave("samples-normalized.pdf", device='pdf', units="in", scale=S, width=W, height=H,
        ggplot(data_c, aes(x=sample_count,y=simulation/sample_count, color=factor(type), shape=factor(type))) +
        geom_point(size=point_size) +
        geom_line(linewidth=line_size) +
        xlab("Samples count (log-scale)")+
        ylab("Normalized time per sample (log-scale)")+
        labs(color="Sample Count", shape="Sample Count") +
        #scale_color_manual(values=RColorBrewer::brewer.pal(9,'YlGnBu')[2:9]) +
        scale_color_brewer(palette="Paired") +
        scale_y_log10(labels = sisec) +
        scale_x_log10(labels=function(x) paste0(x/10^6,"M"), breaks=c(1*10^6,2*10^6,4*10^6,10*10^6)) +
        theme + background_grid() + theme(legend.position="bottom")
    )
}

{
    data_c = data

    # data_c = subset(data_c, max_time == 100)
    data_c = subset(data_c, nodes == 100)
    data_c = subset(data_c, formula_size == 4)
    data_c = subset(data_c, sample_count == 1000000)

    data_c["formula_size"] =  2 * (data_c["formula_size"] + 1)

    # data_c$sample_count2 = factor(c("1M", "2M", "3M", "4M", "5M", "6M", "7M", "8M", "9M", "10M"), levels=c("1M", "2M", "3M", "4M", "5M", "6M", "7M", "8M", "9M", "10M"))

    ggsave("max-time-normalized.pdf", device='pdf', units="in", scale=S, width=W, height=H,
        ggplot(data_c, aes(x=max_time,y=simulation/max_time, color=factor(type), shape=factor(type))) +
        geom_point(size=point_size) +
        geom_line(linewidth=line_size) +
        xlab("Samples count (log-scale)")+
        ylab("Normalized time per time step (log-scale)")+
        labs(color="Sample Count", shape="Sample Count") +
        #scale_color_manual(values=RColorBrewer::brewer.pal(9,'YlGnBu')[2:9]) +
        scale_color_brewer(palette="Paired") +
        scale_y_log10(labels = sisec) +
        scale_x_log10(breaks=c(100,200,400,800,1000)) +
        theme + background_grid() + theme(legend.position="bottom")
    )
}

{
    data_c = data

    data_c = subset(data_c, max_time == 100)
    data_c = subset(data_c, nodes == 100)
    # data_c = subset(data_c, formula_size == 4)
    data_c = subset(data_c, sample_count == 1000000)

    data_c["formula_size"] =  2 * (data_c["formula_size"] + 1)

    # data_c$sample_count2 = factor(c("1M", "2M", "3M", "4M", "5M", "6M", "7M", "8M", "9M", "10M"), levels=c("1M", "2M", "3M", "4M", "5M", "6M", "7M", "8M", "9M", "10M"))

    ggsave("formulas-normalized.pdf", device='pdf', units="in", scale=S, width=W, height=H,
        ggplot(data_c, aes(x=formula_size,y=simulation/formula_size, color=factor(type), shape=factor(type))) +
        geom_point(size=point_size) +
        geom_line(linewidth=line_size) +
        xlab("Samples count (log-scale)")+
        ylab("Normalized time per time step (log-scale)")+
        labs(color="Sample Count", shape="Sample Count") +
        #scale_color_manual(values=RColorBrewer::brewer.pal(9,'YlGnBu')[2:9]) +
        scale_color_brewer(palette="Paired") +
        scale_y_log10(labels = sisec) +
        scale_x_log10(breaks=c(10,20,40,80,100)) +
        theme + background_grid() + theme(legend.position="bottom")
    )
}